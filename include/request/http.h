#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <vector>

class HttpRequest {
private:
    std::string _method;
    std::string _resource;
    std::string _version;

    std::map<std::string, std::string> _headers;
    std::string _body;

    void parse_header_line(std::string);
    void parse_headers(std::vector<std::string>);
    void parse_body(std::vector<std::string>);
    void parse_request(char* request, int size);

public:
    HttpRequest();

    HttpRequest(char* request, int size);

    std::string method() const;
    std::string resource() const;
    std::string version() const;
    
    std::map<std::string, std::string> headers() const;
    std::string body() const;
};

HttpRequest::HttpRequest() :
    _method{""},
    _resource{""},
    _version{""},
    _headers{std::map<std::string, std::string>()},
    _body{""}
{}

HttpRequest::HttpRequest(char* request, int size) {
    std::cout << "size: " <<  size << std::endl;
    parse_request(request, size);
}

void HttpRequest::parse_request(char* request, int size) {
    std::vector<std::string> lines;

    int cur = 0;
    std::string buffer = "";
    while (cur < size) {
        if (request[cur] != '\n') {
            buffer += request[cur];
        } else {
            lines.push_back(buffer);
            buffer = "";
        }
        cur++;
    }

    parse_header_line(lines[0]);

    std::vector<std::string> headers, body;
    cur = 1;
    int n = lines.size();
    while (cur < n && lines[cur] != "") {
        headers.push_back(lines[cur]);
        cur++;
    }
    parse_headers(headers);

    cur++;
    while (cur < n) {
        body.push_back(lines[cur]);
        cur++;
    }
    parse_body(body);
}

void HttpRequest::parse_header_line(std::string raw_header_line) {
    std::string buffer = "";
    int part = 0;
    for(auto c : raw_header_line) {
        if (c == ' ') {
            std::cout << "buffer: " << buffer << std::endl;
            if (part == 0) _method = buffer;
            if (part == 1) _resource = buffer;
            buffer = "";
            part++;
        } else {
            buffer += c;
        }
    }
    _version = buffer;
}

void HttpRequest::parse_headers(std::vector<std::string> raw_headers) {
    for(std::string header : raw_headers) {
        int delimiter_index = header.find(": ");
        std::string key = header.substr(0, delimiter_index);
        std::string value = header.substr(delimiter_index + 2, header.size());
        _headers.insert({key, value});
    }
}

void HttpRequest::parse_body(std::vector<std::string> raw_body) {
    for(auto header : raw_body) std::cout << header << std::endl;
}

std::string HttpRequest::method() const {
    return _method;
}

std::string HttpRequest::version() const {
    return _version;
}

std::string HttpRequest::resource() const {
    return _resource;
}

std::map<std::string, std::string> HttpRequest::headers() const {
    return _headers;
}

std::string HttpRequest::body() const {
    return _body;
}

#endif