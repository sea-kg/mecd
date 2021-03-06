#ifndef WSJCPP_LIGHT_WEB_HTTP_REQUEST_H
#define WSJCPP_LIGHT_WEB_HTTP_REQUEST_H

#include <string>
#include <map>
#include <vector>

// ---------------------------------------------------------------------

class WsjcppLightWebHttpRequestQueryValue {
    public:
        WsjcppLightWebHttpRequestQueryValue(const std::string &sName, const std::string &sValue);
        std::string getName() const;
        std::string getValue() const;
    private:
        std::string m_sName;
        std::string m_sValue;
};

// ---------------------------------------------------------------------

class WsjcppLightWebHttpRequest {
    public:
        WsjcppLightWebHttpRequest(
            int nSockFd,
            const std::string &sAddress
        );
        ~WsjcppLightWebHttpRequest() {};

        int getSockFd() const;
        std::string getUniqueId() const;
        void appendRecieveRequest(const std::string &sRequestPart);
        bool isEnoughAppendReceived() const;
        
        std::string getAddress() const;
        std::string getRequestType() const;
        std::string getRequestPath() const;
        std::string getRequestBody() const;
        std::string getRequestHttpVersion() const;
        const std::vector<WsjcppLightWebHttpRequestQueryValue> &getRequestQueryParams();

    private:
        std::string TAG;

        void parseFirstLine(const std::string &sHeader);

        enum EnumParserState {
            START,
            BODY,
            ENDED
        };
        int m_nSockFd;
        bool m_bClosed;
        EnumParserState m_nParserState;
        std::vector<std::string> m_vHeaders;
        int m_nContentLength;
        std::string m_sUniqueId;
        std::string m_sRequest;
        std::string m_sAddress;
        std::string m_sRequestType;
        std::string m_sRequestPath;
        std::string m_sRequestBody;
        std::vector<WsjcppLightWebHttpRequestQueryValue> m_vRequestQueryParams;
        std::string m_sRequestHttpVersion;

        std::string m_sResponseCacheControl;
        std::string m_sLastModified;
};

#endif // WSJCPP_LIGHT_WEB_HTTP_REQUEST_H


