#include "light_http_thread_worker.h"
#include <logger.h>
#include <light_http_handlers.h>
#include <light_http_response.h>

// ----------------------------------------------------------------------

void* processRequest(void *arg) {
	LightHttpThreadWorker *pWorker = (LightHttpThreadWorker *)arg;
	pthread_detach(pthread_self());
    pWorker->run();
	return 0;
}

// ----------------------------------------------------------------------

LightHttpThreadWorker::LightHttpThreadWorker(const std::string &sName, LightHttpDequeRequests *pDeque, LightHttpHandlers *pHandlers) {
    TAG = "LightHttpThreadWorker-" + sName;
    m_pDeque = pDeque;
    m_bStop = false;
    m_sName = sName;
    m_pHandlers = pHandlers;
}

// ----------------------------------------------------------------------

void LightHttpThreadWorker::start() {
    m_bStop = false;
    pthread_create(&m_serverThread, NULL, &processRequest, (void *)this);
}

// ----------------------------------------------------------------------

void LightHttpThreadWorker::stop() {
    m_bStop = true;
}

// ----------------------------------------------------------------------

void LightHttpThreadWorker::run() {
    const int nMaxPackageSize = 4096;
    while(1) {
		if (m_bStop) return;
        LightHttpRequest *pInfo = m_pDeque->popRequest();
		bool bExists = pInfo != nullptr;
        // TODO refactor
		if (bExists) {
            int nSockFd = pInfo->sockFd();
            LightHttpResponse *pResponse = new LightHttpResponse(nSockFd);
            int n;
            // int newsockfd = (long)arg;
            char msg[nMaxPackageSize];
            memset(msg, 0, nMaxPackageSize);

            std::string sRequest;
            
            // std::cout << nSockFd  << ": address = " << info->address() << "\n";
            // read data from socket
            while(1) { // problem can be here
                // std::cout << nSockFd  << ": wait recv...\n";
                n = recv(nSockFd, msg, nMaxPackageSize, 0);
                // std::cout << "N: " << n << std::endl;
                if (n == -1) {
                    std::cout << nSockFd  << ": error read... \n";
                    break;
                }
                if (n == 0) {
                //close(nSockFd);
                break;
                }
                msg[n]=0;
                //send(newsockfd,msg,n,0);
                sRequest = std::string(msg);
                // stop reading
                int len = sRequest.length();
                // TODO  read body
                if (len > 4 && ((sRequest[len-1] == '\n' && sRequest[len-2] == '\r' && sRequest[len-3] == '\n' && sRequest[len-4] == '\r')
                        || (sRequest[len-1] == '\n' && sRequest[len-2] == '\n'))
                ) {
                    // std::cout << nSockFd  << ": end of request\n";
                    break;
                }
                // usleep(100);
            }
            Log::info(TAG, "Request: \n" + sRequest);
            pInfo->parseRequest(sRequest);
            // std::cout << nSockFd  << ": request >>>> \n" << sRequest << "\n <<<<< request\n";
            
            if (pInfo->requestType() == "OPTIONS") {
                pResponse->ok().sendOptions("OPTIONS, GET, POST");
            } else if (pInfo->requestType() != "GET" && pInfo->requestType() != "POST") {
                pResponse->notImplemented().sendEmpty();
            } else {
                if (!m_pHandlers->handle(m_sName, pInfo)) {
                    pResponse->notFound().sendEmpty();
                } else {
                    // TODO resp internal error
                    // this->response(LightHttpResponse::RESP_INTERNAL_SERVER_ERROR);     
                }
            }
			delete pInfo;
            delete pResponse;
		}

		if (!bExists) {
            if (m_bStop) return;
			usleep(100);
            if (m_bStop) return;
		}
	}
}
    