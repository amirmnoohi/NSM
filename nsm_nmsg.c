#include "nsm_nmsg.h"

/* ==================CLIENT CODE=====================*/
struct nmsg_client_t
{
    struct socket *sock;
    struct sockaddr_in dest_addr;
    struct msghdr msg;
    struct kvec vec;
} nmsg_client;

// Function to initialize client variables
int nmsg_init_client(const char *ip_address, const int port_number)
{
    int err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &(nmsg_client.sock));
    if (err < 0)
        return err;

    memset(&(nmsg_client.dest_addr), 0, sizeof(struct sockaddr_in));
    nmsg_client.dest_addr.sin_family = AF_INET;
    nmsg_client.dest_addr.sin_port = htons(port_number);
    nmsg_client.dest_addr.sin_addr.s_addr = in_aton(ip_address);

    return 0;
}

// Function to send message to server
int nmsg_send_message(char *message)
{
    memset(&(nmsg_client.msg), 0, sizeof(nmsg_client.msg));
    nmsg_client.msg.msg_name = &nmsg_client.dest_addr;
    nmsg_client.msg.msg_namelen = sizeof(nmsg_client.dest_addr);

    nmsg_client.vec.iov_base = message;
    nmsg_client.vec.iov_len = strlen(message);

    if (kernel_sendmsg(nmsg_client.sock, &nmsg_client.msg, &nmsg_client.vec, 1, strlen(message)) < 0)
        return -1;

    return 0;
}

// Function to destroy client variables
void nmsg_destroy_client(void)
{
    sock_release(nmsg_client.sock);
}
/* ==================CLIENT CODE=====================*/

/* ==================SERVER CODE=====================*/
struct nmsg_server_t
{
    struct socket *sock;
    struct sockaddr_in sin;
    char *buf;
    struct msghdr msg;
    struct kvec vec;
    char ip_address[100];
    int port_number;
    struct task_struct *thread;
} nmsg_server;

// Function to initialize server variables
int __nmsg_init_server(void * data){
    int len;

    int err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &(nmsg_server.sock));
    if (err < 0) {
        printk(KERN_ERR "Failed to create UDP socket: %d\n", err);
        return err;
    }
    
    // Bind the socket to port 12345
    memset(&(nmsg_server.sin), 0, sizeof(struct sockaddr_in));
    nmsg_server.sin.sin_family = AF_INET;
    nmsg_server.sin.sin_port = htons(nmsg_server.port_number);

    in4_pton(nmsg_server.ip_address, -1, (u8 *)&(nmsg_server.sin).sin_addr.s_addr, '\0', NULL);

    err = nmsg_server.sock->ops->bind(nmsg_server.sock, (struct sockaddr *)&(nmsg_server.sin), sizeof(struct sockaddr_in));
    if (err < 0) {
        printk(KERN_ERR "Failed to bind UDP socket: %d\n", err);
        goto err_out;
    }

    nmsg_server.buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!nmsg_server.buf) {
        printk(KERN_ERR "Failed to allocate receive buffer\n");
        goto err_out;
    }

    // Receive and process incoming messages
    while (!kthread_should_stop()) {
        memset(&(nmsg_server.msg), 0, sizeof(nmsg_server.msg));
        nmsg_server.msg.msg_name = NULL;
        nmsg_server.msg.msg_namelen = 0;

        nmsg_server.vec.iov_base = nmsg_server.buf;
        nmsg_server.vec.iov_len = PAGE_SIZE;

        // Set the thread state to TASK_INTERRUPTIBLE
        set_current_state(TASK_INTERRUPTIBLE);

        len = kernel_recvmsg(nmsg_server.sock, &(nmsg_server.msg), &(nmsg_server.vec), 0, 1024, MSG_WAITALL);

        // If a termination signal was received, break out of the loop
        if (kthread_should_stop()) {
            break;
        }

        // Set the thread state back to TASK_RUNNING
        set_current_state(TASK_RUNNING);

        if (len < 0) {
            printk(KERN_ERR "Failed to receive message: %d\n", len);
            break;
        }

        printk(KERN_INFO "Received %d bytes: %s\n", len, nmsg_server.buf);

        // Process the received message
        // ...
    }


    kfree(nmsg_server.buf);
err_out:
    sock_release(nmsg_server.sock);
    return err;
}

// Function to start server
int nmsg_start_server(const char *ip_address, const int port_number){
    // copy ip_address to nmsg_server.ip_address
    strncpy(nmsg_server.ip_address, ip_address, strlen(ip_address));
    nmsg_server.port_number = port_number;

    // Create a kernel thread to run the UDP server
    nmsg_server.thread = kthread_create(__nmsg_init_server, NULL, "nmsg_server");
    if (IS_ERR(nmsg_server.thread)) {
        printk(KERN_ERR "Failed to create UDP server thread\n");
        return PTR_ERR(nmsg_server.thread);
    }

    wake_up_process(nmsg_server.thread);
    return 0;
}

// Function to stop server
void nmsg_stop_server(void){
    if (nmsg_server.thread) {
        send_sig(SIGKILL, nmsg_server.thread, 1);
    }

    // free nmsg_server
    kfree(nmsg_server.buf);
    sock_release(nmsg_server.sock);
}

/* ==================SERVER CODE=====================*/
