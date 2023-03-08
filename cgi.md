* The webserver receives requests and forward them to the CGI.

The two most common way a CGI program may call:
1 . by using an html form --> REQUEST_METHOD=POST
2 . by using a get method --> REQUEST_METHOD=GET


The CGI has to perform the following three tasks in order to retrieve the necessary infos:
* Find out the REQUEST_METHOD used by the client.
* If the REQUEST_METHOD used was the GET method, the CGI program knows that all additional values may be retrieved from the	QUERY_STRING environment variable.
* If the REQUEST_METHOD used was POST, the CGI knows that additional information where passed through STDIN, it will have then to query the CONTENT_LENGTH environment variable to know how much information it will read from STDIN