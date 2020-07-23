# SK_testhttp

Repo consists of program testhttp_raw containing client-side software and testhttp script. This two parts are providing tools to simple testing WWW websites. This tools in applications layer use HTTP protocol in 1.1 version, TCP in transportation layer and IPv4 in network layer.

Client after interpreteting arguments passed by command line, connects with designated address and port, sends to server appropriate HTTP request, receives HTTP response from server, analyzes result and shows action raport.

## Comunication descrption

Technical description of HTTP request and response format can be found in document RFC7230. Client connects with desired address and port, then sends appropriate HTTP request. Connection address and port passed in command line don't have to match address and port of tested WWW website. 

If server response differ from 200 OK (e.g. 202 Accepted), client prints raport consisting of HTTP response status line. Otherwise, raport consists of two parts: website cookies and real length of sent recource. Technical description of format of Set-Cookie header can be found in document RFC2109. All restrictions according to HTTP protocola in this implementation are consistent with generl use solutions. Length of recource is counted properly also when resource is sent in chunked encoding. 

## Raport description

Raport consists of two consecutive parts: website cookies and real length of sent resource. Website cookies are presented in as many lines as there were cookies. Each cookie is printed in individual line in key=value format. Real length of sent resource is presented in one, individual line after string "Dlugosc zasobu:". Length of sent resource is printed in bytes in dcimal format.

## Command line description

General lauching of program looks as follows:


    testhttp_raw <connection address>:<port> <cookies file> <tested http address>

    
where

    <connection address> - address with which client is going to connect;
    <port> - port number with which client is going to connect;
    <cookies file> - file consisting cookies designated to sent to HTTP server, description of cookies file format below;
    <tested http address> - address of WWW website, which is going to be served by HTTP server.

In cookies file every cookie is in individual line in key=value format (without whitespaces around '=' sign)

Exemplary runs:

  ./testhttp_raw www.mimuw.edu.pl:80 cookies.txt http://www.mimuw.edu.pl/;

  ./testhttp_raw 127.0.0.1:30303 ciasteczka.txt https://www.mimuw.edu.pl/;

  ./testhttp <cookies file> <tested http address>.
