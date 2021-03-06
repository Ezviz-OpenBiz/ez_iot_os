// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _EZ_HTTP_SERVER_H_
#define _EZ_HTTP_SERVER_H_

#include "ezos_def.h"
#include "http_parser.h"

#ifdef __cplusplus
extern "C"
{
#endif

    #define EZHTTPD_ERRNO_BASE  0x00050000

    typedef enum
    {
        EZHTTPD_ERRNO_FAIL = -1,
        EZHTTPD_ERRNO_SUCC = 0,
        EZHTTPD_ERRNO_HANDLERS_FULL = EZHTTPD_ERRNO_BASE + 0x01,  ///< All slots for registering URI handlers have been consumed
        EZHTTPD_ERRNO_HANDLER_EXISTS = EZHTTPD_ERRNO_BASE + 0x02, ///< URI handler with same method and target URI already registered
        EZHTTPD_ERRNO_INVALID_REQ = EZHTTPD_ERRNO_BASE + 0x03,    ///< Invalid request pointer
        EZHTTPD_ERRNO_RESULT_TRUNC = EZHTTPD_ERRNO_BASE + 0x04,   ///< Result string truncated
        EZHTTPD_ERRNO_RESP_HDR = EZHTTPD_ERRNO_BASE + 0x05,       ///< Response header field larger than supported
        EZHTTPD_ERRNO_RESP_SEND = EZHTTPD_ERRNO_BASE + 0x06,      ///< Error occured while sending response packet
        EZHTTPD_ERRNO_ALLOC_MEM = EZHTTPD_ERRNO_BASE + 0x07,      ///< Failed to dynamically allocate memory for resource
        EZHTTPD_ERRNO_TASK = EZHTTPD_ERRNO_BASE + 0x08,           ///< Failed to launch server task/thread
        EZHTTPD_ERRNO_NOT_FOUND = EZHTTPD_ERRNO_BASE + 0x08,      ///< Failed to launch server task/thread
    } ezhttpd_errno_e;

/*
note: esp_https_server.h includes a customized copy of this
initializer that should be kept in sync
*/
#define HTTPD_DEFAULT_CONFIG()                \
    {                                         \
        .task_priority = 5,                   \
        .stack_size = CONFIG_EZIOT_COMPONENT_EZHTTPD_STACK_SIZE,                   \
        .server_port = CONFIG_EZIOT_COMPONENT_EZHTTPD_SERVER_PORT,                    \
        .ctrl_port = 32768,                   \
        .max_open_sockets = 7,                \
        .max_uri_handlers = 8,                \
        .max_resp_headers = 8,                \
        .backlog_conn = 5,                    \
        .lru_purge_enable = ez_false,            \
        .recv_wait_timeout = 5,               \
        .send_wait_timeout = 5,               \
        .global_user_ctx = NULL,              \
        .global_user_ctx_free_fn = NULL,      \
        .global_transport_ctx = NULL,         \
        .global_transport_ctx_free_fn = NULL, \
        .open_fn = NULL,                      \
        .close_fn = NULL,                     \
    }

    /* ************** Group: Initialization ************** */
    /** @name Initialization
     * APIs related to the Initialization of the web server
     * @{
     */

    /**
     * @brief   HTTP Server Instance Handle
     *
     * Every instance of the server will have a unique handle.
     */
    typedef void *httpd_handle_t;

    /**
 * @brief   HTTP Method Type wrapper over "enum http_method"
 *          available in "http_parser" library
 */
    typedef enum http_method httpd_method_t;

    /**
 * @brief  Prototype for freeing context data (if any)
 * @param[in] ctx : object to free
 */
    typedef void (*httpd_free_ctx_fn_t)(void *ctx);

    /**
 * @brief  Function prototype for opening a session.
 *
 * Called immediately after the socket was opened to set up the send/recv functions and
 * other parameters of the socket.
 *
 * @param[in] hd : server instance
 * @param[in] sockfd : session socket file descriptor
 * @return status
 */
    typedef ez_err_t (*httpd_open_func_t)(httpd_handle_t hd, ez_int32_t sockfd);

    /**
 * @brief  Function prototype for closing a session.
 *
 * @note   It's possible that the socket descriptor is invalid at this point, the function
 *         is called for all terminated sessions. Ensure proper handling of return codes.
 *
 * @param[in] hd : server instance
 * @param[in] sockfd : session socket file descriptor
 */
    typedef void (*httpd_close_func_t)(httpd_handle_t hd, ez_int32_t sockfd);

    /**
 * @brief   HTTP Server Configuration Structure
 *
 * @note    Use HTTPD_DEFAULT_CONFIG() to initialize the configuration
 *          to a default value and then modify only those fields that are
 *          specifically determined by the use case.
 */
    typedef struct httpd_config
    {
        unsigned task_priority; /*!< Priority of FreeRTOS task which runs the server */
        ez_size_t stack_size;      /*!< The maximum stack size allowed for the server task */

        /**
     * TCP Port number for receiving and transmitting HTTP traffic
     */
        ez_uint16_t server_port;

        /**
     * UDP Port number for asynchronously exchanging control signals
     * between various components of the server
     */
        ez_uint16_t ctrl_port;

        ez_uint16_t max_open_sockets;  /*!< Max number of sockets/clients connected at any time*/
        ez_uint16_t max_uri_handlers;  /*!< Maximum allowed uri handlers */
        ez_uint16_t max_resp_headers;  /*!< Maximum allowed additional headers in HTTP response */
        ez_uint16_t backlog_conn;      /*!< Number of backlog connections */
        ez_bool_t lru_purge_enable;      /*!< Purge "Least Recently Used" connection */
        ez_uint16_t recv_wait_timeout; /*!< Timeout for recv function (in seconds)*/
        ez_uint16_t send_wait_timeout; /*!< Timeout for send function (in seconds)*/

        /**
     * Global user context.
     *
     * This field can be used to store arbitrary user data within the server context.
     * The value can be retrieved using the server handle, available e.g. in the httpd_req_t struct.
     *
     * When shutting down, the server frees up the user context by
     * calling free() on the global_user_ctx field. If you wish to use a custom
     * function for freeing the global user context, please specify that here.
     */
        void *global_user_ctx;

        /**
     * Free function for global user context
     */
        httpd_free_ctx_fn_t global_user_ctx_free_fn;

        /**
     * Global transport context.
     *
     * Similar to global_user_ctx, but used for session encoding or encryption (e.g. to hold the SSL context).
     * It will be freed using free(), unless global_transport_ctx_free_fn is specified.
     */
        void *global_transport_ctx;

        /**
     * Free function for global transport context
     */
        httpd_free_ctx_fn_t global_transport_ctx_free_fn;

        /**
     * Custom session opening callback.
     *
     * Called on a new session socket just after accept(), but before reading any data.
     *
     * This is an opportunity to set up e.g. SSL encryption using global_transport_ctx
     * and the send/recv/pending session overrides.
     *
     * If a context needs to be maintained between these functions, store it in the session using
     * httpd_sess_set_transport_ctx() and retrieve it later with httpd_sess_get_transport_ctx()
     */
        httpd_open_func_t open_fn;

        /**
     * Custom session closing callback.
     *
     * Called when a session is deleted, before freeing user and transport contexts and before
     * closing the socket. This is a place for custom de-init code common to all sockets.
     *
     * Set the user or transport context to NULL if it was freed here, so the server does not
     * try to free it again.
     *
     * This function is run for all terminated sessions, including sessions where the socket
     * was closed by the network stack - that is, the file descriptor may not be valid anymore.
     */
        httpd_close_func_t close_fn;
    } httpd_config_t;

    /**
 * @brief Starts the web server
 *
 * Create an instance of HTTP server and allocate memory/resources for it
 * depending upon the specified configuration.
 *
 * Example usage:
 * @code{c}
 *
 * //Function for starting the webserver
 * httpd_handle_t start_webserver(void)
 * {
 *      // Generate default configuration
 *      httpd_config_t config = HTTPD_DEFAULT_CONFIG();
 *
 *      // Empty handle to http_server
 *      httpd_handle_t server = NULL;
 *
 *      // Start the httpd server
 *      if (httpd_start(&server, &config) == ez_errno_succ) {
 *          // Register URI handlers
 *          httpd_register_uri_handler(server, &uri_get);
 *          httpd_register_uri_handler(server, &uri_post);
 *      }
 *      // If server failed to start, handle will be NULL
 *      return server;
 * }
 *
 * @endcode
 *
 * @param[in]  config : Configuration for new instance of the server
 * @param[out] handle : Handle to newly created instance of the server. NULL on error
 * @return
 *  - ez_errno_succ    : Instance created successfully
 *  - ez_errno_param_invalid      : Null argument(s)
 *  - ez_errno_httpd_alloc_mem  : Failed to allocate memory for instance
 *  - ez_errno_httpd_task       : Failed to launch server task
 */
    ez_err_t httpd_start(httpd_handle_t *handle, const httpd_config_t *config);

    /**
 * @brief Stops the web server
 *
 * Deallocates memory/resources used by an HTTP server instance and
 * deletes it. Once deleted the handle can no longer be used for accessing
 * the instance.
 *
 * Example usage:
 * @code{c}
 *
 * // Function for stopping the webserver
 * void stop_webserver(httpd_handle_t server)
 * {
 *      // Ensure handle is non NULL
 *      if (server != NULL) {
 *          // Stop the httpd server
 *          httpd_stop(server);
 *      }
 * }
 *
 * @endcode
 *
 * @param[in] handle Handle to server returned by httpd_start
 * @return
 *  - ez_errno_succ : Server stopped successfully
 *  - ez_errno_param_invalid : Handle argument is Null
 */
    ez_err_t httpd_stop(httpd_handle_t handle);

/** End of Group Initialization
 * @}
 */

/* ************** Group: URI Handlers ************** */
/** @name URI Handlers
 * APIs related to the URI handlers
 * @{
 */

/* Max supported HTTP request header length */
#define HTTPD_MAX_REQ_HDR_LEN 512

/* Max supported HTTP request URI length */
#define HTTPD_MAX_URI_LEN 512
    /**
 * @brief HTTP Request Data Structure
 */
    typedef struct httpd_req
    {
        httpd_handle_t handle;                 /*!< Handle to server instance */
        ez_int32_t method;                            /*!< The type of HTTP request, -1 if unsupported method */
        const ez_char_t uri[HTTPD_MAX_URI_LEN + 1]; /*!< The URI of this request (1 byte extra for null termination) */
        ez_size_t content_len;                    /*!< Length of the request body */
        void *aux;                             /*!< Internally used members */

        /**
     * User context pointer passed during URI registration.
     */
        void *user_ctx;

        /**
     * Session Context Pointer
     *
     * A session context. Contexts are maintained across 'sessions' for a
     * given open TCP connection. One session could have multiple request
     * responses. The web server will ensure that the context persists
     * across all these request and responses.
     *
     * By default, this is NULL. URI Handlers can set this to any meaningful
     * value.
     *
     * If the underlying socket gets closed, and this pointer is non-NULL,
     * the web server will free up the context by calling free(), unless
     * free_ctx function is set.
     */
        void *sess_ctx;

        /**
     * Pointer to free context hook
     *
     * Function to free session context
     *
     * If the web server's socket closes, it frees up the session context by
     * calling free() on the sess_ctx member. If you wish to use a custom
     * function for freeing the session context, please specify that here.
     */
        httpd_free_ctx_fn_t free_ctx;
    } httpd_req_t;

    /**
 * @brief Structure for URI handler
 */
    typedef struct httpd_uri
    {
        const ez_char_t *uri;       /*!< The URI to handle */
        httpd_method_t method; /*!< Method supported by the URI */

        /**
     * Handler to call for supported request method. This must
     * return ez_errno_succ, or else the underlying socket will be closed.
     */
        ez_err_t (*handler)(httpd_req_t *r);

        /**
     * Pointer to user context data which will be available to handler
     */
        void *user_ctx;
    } httpd_uri_t;

    /**
 * @brief   Registers a URI handler
 *
 * @note    URI handlers can be registered in real time as long as the
 *          server handle is valid.
 *
 * Example usage:
 * @code{c}
 *
 * ez_err_t my_uri_handler(httpd_req_t* req)
 * {
 *     // Recv , Process and Send
 *     ....
 *     ....
 *     ....
 *
 *     // Fail condition
 *     if (....) {
 *         // Return fail to close session //
 *         return ez_errno_fail;
 *     }
 *
 *     // On success
 *     return ez_errno_succ;
 * }
 *
 * // URI handler structure
 * httpd_uri_t my_uri {
 *     .uri      = "/my_uri/path/xyz",
 *     .method   = HTTPD_GET,
 *     .handler  = my_uri_handler,
 *     .user_ctx = NULL
 * };
 *
 * // Register handler
 * if (httpd_register_uri_handler(server_handle, &my_uri) != ez_errno_succ) {
 *    // If failed to register handler
 *    ....
 * }
 *
 * @endcode
 *
 * @param[in] handle      handle to HTTPD server instance
 * @param[in] uri_handler pointer to handler that needs to be registered
 *
 * @return
 *  - ez_errno_succ : On successfully registering the handler
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_handlers_full  : If no slots left for new handler
 *  - ez_errno_httpd_handler_exists : If handler with same URI and
 *                                   method is already registered
 */
    ez_err_t httpd_register_uri_handler(httpd_handle_t handle,
                                        const httpd_uri_t *uri_handler);

    /**
 * @brief   Unregister a URI handler
 *
 * @param[in] handle    handle to HTTPD server instance
 * @param[in] uri       URI string
 * @param[in] method    HTTP method
 *
 * @return
 *  - ez_errno_succ : On successfully deregistering the handler
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_not_found   : Handler with specified URI and method not found
 */
    ez_err_t httpd_unregister_uri_handler(httpd_handle_t handle,
                                          const ez_char_t *uri, httpd_method_t method);

    /**
 * @brief   Unregister all URI handlers with the specified uri string
 *
 * @param[in] handle   handle to HTTPD server instance
 * @param[in] uri      uri string specifying all handlers that need
 *                     to be deregisterd
 *
 * @return
 *  - ez_errno_succ : On successfully deregistering all such handlers
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_not_found   : No handler registered with specified uri string
 */
    ez_err_t httpd_unregister_uri(httpd_handle_t handle, const ez_char_t *uri);

    /** End of URI Handlers
 * @}
 */

    /* ************** Group: TX/RX ************** */
    /** @name TX / RX
 * Prototype for HTTPDs low-level send/recv functions
 * @{
 */

#define HTTPD_SOCK_ERR_FAIL -1
#define HTTPD_SOCK_ERR_INVALID -2
#define HTTPD_SOCK_ERR_TIMEOUT -3

    /**
 * @brief  Prototype for HTTPDs low-level send function
 *
 * @note   User specified send function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will eventually be conveyed as
 *         return value of httpd_send() function
 *
 * @param[in] hd      : server instance
 * @param[in] sockfd  : session socket file descriptor
 * @param[in] buf     : buffer with bytes to send
 * @param[in] buf_len : data size
 * @param[in] flags   : flags for the send() function
 * @return
 *  - Bytes : The number of bytes sent successfully
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket send()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket send()
 */
    typedef ez_int32_t (*httpd_send_func_t)(httpd_handle_t hd, ez_int32_t sockfd, const ez_char_t *buf, ez_size_t buf_len, ez_int32_t flags);

    /**
 * @brief  Prototype for HTTPDs low-level recv function
 *
 * @note   User specified recv function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will eventually be conveyed as
 *         return value of httpd_req_recv() function
 *
 * @param[in] hd      : server instance
 * @param[in] sockfd  : session socket file descriptor
 * @param[in] buf     : buffer with bytes to send
 * @param[in] buf_len : data size
 * @param[in] flags   : flags for the send() function
 * @return
 *  - Bytes : The number of bytes received successfully
 *  - 0     : Buffer length parameter is zero / connection closed by peer
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket recv()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket recv()
 */
    typedef ez_int32_t (*httpd_recv_func_t)(httpd_handle_t hd, ez_int32_t sockfd, ez_char_t *buf, ez_size_t buf_len, ez_int32_t flags);

    /**
 * @brief  Prototype for HTTPDs low-level "get pending bytes" function
 *
 * @note   User specified pending function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will be handled accordingly in
 *         the server task.
 *
 * @param[in] hd : server instance
 * @param[in] sockfd : session socket file descriptor
 * @return
 *  - Bytes : The number of bytes waiting to be received
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket pending()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket pending()
 */
    typedef ez_int32_t (*httpd_pending_func_t)(httpd_handle_t hd, ez_int32_t sockfd);

    /** End of TX / RX
 * @}
 */

    /* ************** Group: Request/Response ************** */
    /** @name Request / Response
 * APIs related to the data send/receive by URI handlers.
 * These APIs are supposed to be called only from the context of
 * a URI handler where httpd_req_t* request pointer is valid.
 * @{
 */

    /**
 * @brief   Override web server's receive function (by session FD)
 *
 * This function overrides the web server's receive function. This same function is
 * used to read HTTP request packets.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd        HTTPD instance handle
 * @param[in] sockfd    Session socket FD
 * @param[in] recv_func The receive function to be set for this session
 *
 * @return
 *  - ez_errno_succ : On successfully registering override
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_sess_set_recv_override(httpd_handle_t hd, ez_int32_t sockfd, httpd_recv_func_t recv_func);

    /**
 * @brief   Override web server's send function (by session FD)
 *
 * This function overrides the web server's send function. This same function is
 * used to send out any response to any HTTP request.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd        HTTPD instance handle
 * @param[in] sockfd    Session socket FD
 * @param[in] send_func The send function to be set for this session
 *
 * @return
 *  - ez_errno_succ : On successfully registering override
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_sess_set_send_override(httpd_handle_t hd, ez_int32_t sockfd, httpd_send_func_t send_func);

    /**
 * @brief   Override web server's pending function (by session FD)
 *
 * This function overrides the web server's pending function. This function is
 * used to test for pending bytes in a socket.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd           HTTPD instance handle
 * @param[in] sockfd       Session socket FD
 * @param[in] pending_func The receive function to be set for this session
 *
 * @return
 *  - ez_errno_succ : On successfully registering override
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_sess_set_pending_override(httpd_handle_t hd, ez_int32_t sockfd, httpd_pending_func_t pending_func);

    /**
 * @brief   Get the Socket Descriptor from the HTTP request
 *
 * This API will return the socket descriptor of the session for
 * which URI handler was executed on reception of HTTP request.
 * This is useful when user wants to call functions that require
 * session socket fd, from within a URI handler, ie. :
 *      httpd_sess_get_ctx(),
 *      httpd_sess_trigger_close(),
 *      httpd_sess_update_lru_counter().
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid.
 *
 * @param[in] r The request whose socket descriptor should be found
 *
 * @return
 *  - Socket descriptor : The socket descriptor for this request
 *  - -1 : Invalid/NULL request pointer
 */
    ez_int32_t httpd_req_to_sockfd(httpd_req_t *r);

    /**
 * @brief   API to read content data from the HTTP request
 *
 * This API will read HTTP content data from the HTTP request into
 * provided buffer. Use content_len provided in httpd_req_t structure
 * to know the length of data to be fetched. If content_len is too
 * large for the buffer then user may have to make multiple calls to
 * this function, each time fetching 'buf_len' number of bytes,
 * while the pointer to content data is incremented internally by
 * the same number.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - If an error is returned, the URI handler must further return an error.
 *    This will ensure that the erroneous socket is closed and cleaned up by
 *    the web server.
 *  - Presently Chunked Encoding is not supported
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Pointer to a buffer that the data will be read into
 * @param[in] buf_len   Length of the buffer
 *
 * @return
 *  - Bytes : Number of bytes read into the buffer successfully
 *  - 0     : Buffer length parameter is zero / connection closed by peer
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket recv()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket recv()
 */
    ez_int32_t httpd_req_recv(httpd_req_t *r, ez_char_t *buf, ez_size_t buf_len);

    /**
 * @brief   Search for a field in request headers and
 *          return the string length of it's value
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once httpd_resp_send() API is called all request headers
 *    are purged, so request headers need be copied into separate
 *    buffers if they are required later.
 *
 * @param[in]  r        The request being responded to
 * @param[in]  field    The header field to be searched in the request
 *
 * @return
 *  - Length    : If field is found in the request URL
 *  - Zero      : Field not found / Invalid request / Null arguments
 */
    ez_size_t httpd_req_get_hdr_value_len(httpd_req_t *r, const ez_char_t *field);

    /**
 * @brief   Get the value string of a field from the request headers
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once httpd_resp_send() API is called all request headers
 *    are purged, so request headers need be copied into separate
 *    buffers if they are required later.
 *  - If output size is greater than input, then the value is truncated,
 *    accompanied by truncation error as return value.
 *  - Use httpd_req_get_hdr_value_len() to know the right buffer length
 *
 * @param[in]  r        The request being responded to
 * @param[in]  field    The field to be searched in the header
 * @param[out] val      Pointer to the buffer into which the value will be copied if the field is found
 * @param[in]  val_size Size of the user buffer "val"
 *
 * @return
 *  - ez_errno_succ : Field found in the request header and value string copied
 *  - ez_errno_httpd_not_found          : Key not found
 *  - ez_errno_param_invalid        : Null arguments
 *  - ez_errno_httpd_invalid_req  : Invalid HTTP request pointer
 *  - ez_errno_httpd_result_trunc : Value string truncated
 */
    ez_err_t httpd_req_get_hdr_value_str(httpd_req_t *r, const ez_char_t *field, ez_char_t *val, ez_size_t val_size);

    /**
 * @brief   Get Query string length from the request URL
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid
 *
 * @param[in]  r    The request being responded to
 *
 * @return
 *  - Length    : Query is found in the request URL
 *  - Zero      : Query not found / Null arguments / Invalid request
 */
    ez_size_t httpd_req_get_url_query_len(httpd_req_t *r);

    /**
 * @brief   Get Query string from the request URL
 *
 * @note
 *  - Presently, the user can fetch the full URL query string, but decoding
 *    will have to be performed by the user. Request headers can be read using
 *    httpd_req_get_hdr_value_str() to know the 'Content-Type' (eg. Content-Type:
 *    application/x-www-form-urlencoded) and then the appropriate decoding
 *    algorithm needs to be applied.
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid
 *  - If output size is greater than input, then the value is truncated,
 *    accompanied by truncation error as return value
 *  - Use httpd_req_get_url_query_len() to know the right buffer length
 *
 * @param[in]  r         The request being responded to
 * @param[out] buf       Pointer to the buffer into which the query string will be copied (if found)
 * @param[in]  buf_len   Length of output buffer
 *
 * @return
 *  - ez_errno_succ : Query is found in the request URL and copied to buffer
 *  - ez_errno_httpd_not_found          : Query not found
 *  - ez_errno_param_invalid        : Null arguments
 *  - ez_errno_httpd_invalid_req  : Invalid HTTP request pointer
 *  - ez_errno_httpd_result_trunc : Query string truncated
 */
    ez_err_t httpd_req_get_url_query_str(httpd_req_t *r, ez_char_t *buf, ez_size_t buf_len);

    /**
 * @brief   Helper function to get a URL query tag from a query
 *          string of the type param1=val1&param2=val2
 *
 * @note
 *  - The components of URL query string (keys and values) are not URLdecoded.
 *    The user must check for 'Content-Type' field in the request headers and
 *    then depending upon the specified encoding (URLencoded or otherwise) apply
 *    the appropriate decoding algorithm.
 *  - If actual value size is greater than val_size, then the value is truncated,
 *    accompanied by truncation error as return value.
 *
 * @param[in]  qry       Pointer to query string
 * @param[in]  key       The key to be searched in the query string
 * @param[out] val       Pointer to the buffer into which the value will be copied if the key is found
 * @param[in]  val_size  Size of the user buffer "val"
 *
 * @return
 *  - ez_errno_succ : Key is found in the URL query string and copied to buffer
 *  - ez_errno_httpd_not_found          : Key not found
 *  - ez_errno_param_invalid        : Null arguments
 *  - ez_errno_httpd_result_trunc : Value string truncated
 */
    ez_err_t httpd_query_key_value(const ez_char_t *qry, const ez_char_t *key, ez_char_t *val, ez_size_t val_size);

    /**
 * @brief   API to send a complete HTTP response.
 *
 * This API will send the data as an HTTP response to the request.
 * This assumes that you have the entire response ready in a single
 * buffer. If you wish to send response in incremental chunks use
 * httpd_resp_send_chunk() instead.
 *
 * If no status code and content-type were set, by default this
 * will send 200 OK status code and content type as text/html.
 * You may call the following functions before this API to configure
 * the response headers :
 *      httpd_resp_set_status() - for setting the HTTP status string,
 *      httpd_resp_set_type()   - for setting the Content Type,
 *      httpd_resp_set_hdr()    - for appending any additional field
 *                                value entries in the response header
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, the request has been responded to.
 *  - No additional data can then be sent for the request.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Buffer from where the content is to be fetched
 * @param[in] buf_len   Length of the buffer, -1 to use strlen()
 *
 * @return
 *  - ez_errno_succ : On successfully sending the response packet
 *  - ez_errno_param_invalid : Null request pointer
 *  - ez_errno_httpd_resp_hdr    : Essential headers are too large for internal buffer
 *  - ez_errno_httpd_resp_send   : Error in raw send
 *  - ez_errno_httpd_invalid_req : Invalid request
 */
    ez_err_t httpd_resp_send(httpd_req_t *r, const ez_char_t *buf, ez_ssize_t buf_len);

    /**
 * @brief   API to send one HTTP chunk
 *
 * This API will send the data as an HTTP response to the
 * request. This API will use chunked-encoding and send the response
 * in the form of chunks. If you have the entire response contained in
 * a single buffer, please use httpd_resp_send() instead.
 *
 * If no status code and content-type were set, by default this will
 * send 200 OK status code and content type as text/html. You may
 * call the following functions before this API to configure the
 * response headers
 *      httpd_resp_set_status() - for setting the HTTP status string,
 *      httpd_resp_set_type()   - for setting the Content Type,
 *      httpd_resp_set_hdr()    - for appending any additional field
 *                                value entries in the response header
 *
 * @note
 * - This API is supposed to be called only from the context of
 *   a URI handler where httpd_req_t* request pointer is valid.
 * - When you are finished sending all your chunks, you must call
 *   this function with buf_len as 0.
 * - Once this API is called, all request headers are purged, so
 *   request headers need be copied into separate buffers if they
 *   are required later.
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Pointer to a buffer that stores the data
 * @param[in] buf_len   Length of the data from the buffer that should be sent out, -1 to use strlen()
 *
 * @return
 *  - ez_errno_succ : On successfully sending the response packet chunk
 *  - ez_errno_param_invalid : Null request pointer
 *  - ez_errno_httpd_resp_hdr    : Essential headers are too large for internal buffer
 *  - ez_errno_httpd_resp_send   : Error in raw send
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_send_chunk(httpd_req_t *r, const ez_char_t *buf, ez_ssize_t buf_len);

/* Some commonly used status codes */
#define HTTPD_200 "200 OK"                    /*!< HTTP Response 200 */
#define HTTPD_204 "204 No Content"            /*!< HTTP Response 204 */
#define HTTPD_207 "207 Multi-Status"          /*!< HTTP Response 207 */
#define HTTPD_400 "400 Bad Request"           /*!< HTTP Response 400 */
#define HTTPD_404 "404 Not Found"             /*!< HTTP Response 404 */
#define HTTPD_408 "408 Request Timeout"       /*!< HTTP Response 408 */
#define HTTPD_500 "500 Internal Server Error" /*!< HTTP Response 500 */

    /**
 * @brief   API to set the HTTP status code
 *
 * This API sets the status of the HTTP response to the value specified.
 * By default, the '200 OK' response is sent as the response.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - This API only sets the status to this value. The status isn't
 *    sent out until any of the send APIs is executed.
 *  - Make sure that the lifetime of the status string is valid till
 *    send function is called.
 *
 * @param[in] r         The request being responded to
 * @param[in] status    The HTTP status code of this response
 *
 * @return
 *  - ez_errno_succ : On success
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_set_status(httpd_req_t *r, const ez_char_t *status);

/* Some commonly used content types */
#define HTTPD_TYPE_JSON "application/json"          /*!< HTTP Content type JSON */
#define HTTPD_TYPE_TEXT "text/html"                 /*!< HTTP Content type text/HTML */
#define HTTPD_TYPE_OCTET "application/octet-stream" /*!< HTTP Content type octext-stream */

    /**
 * @brief   API to set the HTTP content type
 *
 * This API sets the 'Content Type' field of the response.
 * The default content type is 'text/html'.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - This API only sets the content type to this value. The type
 *    isn't sent out until any of the send APIs is executed.
 *  - Make sure that the lifetime of the type string is valid till
 *    send function is called.
 *
 * @param[in] r     The request being responded to
 * @param[in] type  The Content Type of the response
 *
 * @return
 *  - ez_errno_succ   : On success
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_set_type(httpd_req_t *r, const ez_char_t *type);

    /**
 * @brief   API to append any additional headers
 *
 * This API sets any additional header fields that need to be sent in the response.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - The header isn't sent out until any of the send APIs is executed.
 *  - The maximum allowed number of additional headers is limited to
 *    value of max_resp_headers in config structure.
 *  - Make sure that the lifetime of the field value strings are valid till
 *    send function is called.
 *
 * @param[in] r     The request being responded to
 * @param[in] field The field name of the HTTP header
 * @param[in] value The value of this HTTP header
 *
 * @return
 *  - ez_errno_succ : On successfully appending new header
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_resp_hdr    : Total additional headers exceed max allowed
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_set_hdr(httpd_req_t *r, const ez_char_t *field, const ez_char_t *value);

    /**
 * @brief   Helper function for HTTP 404
 *
 * Send HTTP 404 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ez_errno_succ : On successfully sending the response packet
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_resp_send   : Error in raw send
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_send_404(httpd_req_t *r);

    /**
 * @brief   Helper function for HTTP 408
 *
 * Send HTTP 408 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ez_errno_succ : On successfully sending the response packet
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_resp_send   : Error in raw send
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_send_408(httpd_req_t *r);

    /**
 * @brief   Helper function for HTTP 500
 *
 * Send HTTP 500 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ez_errno_succ : On successfully sending the response packet
 *  - ez_errno_param_invalid : Null arguments
 *  - ez_errno_httpd_resp_send   : Error in raw send
 *  - ez_errno_httpd_invalid_req : Invalid request pointer
 */
    ez_err_t httpd_resp_send_500(httpd_req_t *r);

    /**
 * @brief   Raw HTTP send
 *
 * Call this API if you wish to construct your custom response packet.
 * When using this, all essential header, eg. HTTP version, Status Code,
 * Content Type and Length, Encoding, etc. will have to be constructed
 * manually, and HTTP delimeters (CRLF) will need to be placed correctly
 * for separating sub-sections of the HTTP response packet.
 *
 * If the send override function is set, this API will end up
 * calling that function eventually to send data out.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Unless the response has the correct HTTP structure (which the
 *    user must now ensure) it is not guaranteed that it will be
 *    recognized by the client. For most cases, you wouldn't have
 *    to call this API, but you would rather use either of :
 *          httpd_resp_send(),
 *          httpd_resp_send_chunk()
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Buffer from where the fully constructed packet is to be read
 * @param[in] buf_len   Length of the buffer
 *
 * @return
 *  - Bytes : Number of bytes that were sent successfully
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket send()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket send()
 */
    ez_int32_t httpd_send(httpd_req_t *r, const ez_char_t *buf, ez_size_t buf_len);

    /** End of Request / Response
 * @}
 */

    /* ************** Group: Session ************** */
    /** @name Session
 * Functions for controlling sessions and accessing context data
 * @{
 */

    /**
 * @brief   Get session context from socket descriptor
 *
 * Typically if a session context is created, it is available to URI handlers
 * through the httpd_req_t structure. But, there are cases where the web
 * server's send/receive functions may require the context (for example, for
 * accessing keying information etc). Since the send/receive function only have
 * the socket descriptor at their disposal, this API provides them with a way to
 * retrieve the session context.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 *
 * @return
 *  - void* : Pointer to the context associated with this session
 *  - NULL  : Empty context / Invalid handle / Invalid socket fd
 */
    void *httpd_sess_get_ctx(httpd_handle_t handle, ez_int32_t sockfd);

    /**
 * @brief   Set session context by socket descriptor
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @param[in] ctx       Context object to assign to the session
 * @param[in] free_fn   Function that should be called to free the context
 */
    void httpd_sess_set_ctx(httpd_handle_t handle, ez_int32_t sockfd, void *ctx, httpd_free_ctx_fn_t free_fn);

    /**
 * @brief   Get session 'transport' context by socket descriptor
 * @see     httpd_sess_get_ctx()
 *
 * This context is used by the send/receive functions, for example to manage SSL context.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @return
 *  - void* : Pointer to the transport context associated with this session
 *  - NULL  : Empty context / Invalid handle / Invalid socket fd
 */
    void *httpd_sess_get_transport_ctx(httpd_handle_t handle, ez_int32_t sockfd);

    /**
 * @brief   Set session 'transport' context by socket descriptor
 * @see     httpd_sess_set_ctx()
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @param[in] ctx       Transport context object to assign to the session
 * @param[in] free_fn   Function that should be called to free the transport context
 */
    void httpd_sess_set_transport_ctx(httpd_handle_t handle, ez_int32_t sockfd, void *ctx, httpd_free_ctx_fn_t free_fn);

    /**
 * @brief   Get HTTPD global user context (it was set in the server config struct)
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @return global user context
 */
    void *httpd_get_global_user_ctx(httpd_handle_t handle);

    /**
 * @brief   Get HTTPD global transport context (it was set in the server config struct)
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @return global transport context
 */
    void *httpd_get_global_transport_ctx(httpd_handle_t handle);

    /**
 * @brief   Trigger an httpd session close externally
 *
 * @note    Calling this API is only required in special circumstances wherein
 *          some application requires to close an httpd client session asynchronously.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor of the session to be closed
 *
 * @return
 *  - ez_errno_succ    : On successfully initiating closure
 *  - ez_errno_fail  : Failure to queue work
 *  - ez_errno_httpd_not_found   : Socket fd not found
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_sess_trigger_close(httpd_handle_t handle, ez_int32_t sockfd);

    /**
 * @brief   Update LRU counter for a given socket
 *
 * LRU Counters are internally associated with each session to monitor
 * how recently a session exchanged traffic. When LRU purge is enabled,
 * if a client is requesting for connection but maximum number of
 * sockets/sessions is reached, then the session having the earliest
 * LRU counter is closed automatically.
 *
 * Updating the LRU counter manually prevents the socket from being purged
 * due to the Least Recently Used (LRU) logic, even though it might not
 * have received traffic for some time. This is useful when all open
 * sockets/session are frequently exchanging traffic but the user specifically
 * wants one of the sessions to be kept open, irrespective of when it last
 * exchanged a packet.
 *
 * @note    Calling this API is only necessary if the LRU Purge Enable option
 *          is enabled.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor of the session for which LRU counter
 *                      is to be updated
 *
 * @return
 *  - ez_errno_succ : Socket found and LRU counter updated
 *  - ez_errno_httpd_not_found   : Socket not found
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_sess_update_lru_counter(httpd_handle_t handle, ez_int32_t sockfd);

    /** End of Session
 * @}
 */

    /* ************** Group: Work Queue ************** */
    /** @name Work Queue
 * APIs related to the HTTPD Work Queue
 * @{
 */

    /**
 * @brief   Prototype of the HTTPD work function
 *          Please refer to httpd_queue_work() for more details.
 * @param[in] arg   The arguments for this work function
 */
    typedef void (*httpd_work_fn_t)(void *arg);

    /**
 * @brief   Queue execution of a function in HTTPD's context
 *
 * This API queues a work function for asynchronous execution
 *
 * @note    Some protocols require that the web server generate some asynchronous data
 *          and send it to the persistently opened connection. This facility is for use
 *          by such protocols.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] work      Pointer to the function to be executed in the HTTPD's context
 * @param[in] arg       Pointer to the arguments that should be passed to this function
 *
 * @return
 *  - ez_errno_succ   : On successfully queueing the work
 *  - ez_errno_fail : Failure in ctrl socket
 *  - ez_errno_param_invalid : Null arguments
 */
    ez_err_t httpd_queue_work(httpd_handle_t handle, httpd_work_fn_t work, void *arg);

    /** End of Group Work Queue
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_HTTP_SERVER_H_ */
