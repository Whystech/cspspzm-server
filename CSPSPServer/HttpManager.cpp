#include "HttpManager.h"

//------------------------------------------------------------------------------------------------
HttpManager::HttpManager()
{
	mSocket = new Socket();
	strcpy(mBuffer,"");
	mTimer = 0.0f;
	mReconnectTimer = 0.0f;
	strcpy(mHost,"");
	strcpy(mHostText,"");
	mPort = 0;
	mContentSize = -1;
	mBufferSize = 0;
	mContentType = CONTENT_UNKNOWN;
}

//------------------------------------------------------------------------------------------------
HttpManager::~HttpManager()
{
	SocketClose(mSocket);
	SocketFree(mSocket);
}

//------------------------------------------------------------------------------------------------
void HttpManager::Connect(char* host, char *hosttext, int port)
{
	strcpy(mHost,host);
	strcpy(mHostText,hosttext);
	mPort = port;
	//SocketConnect(mSocket,host,port); //don't actually connect until a request is made?
	mReconnectTimer = 0.0f;
}

//------------------------------------------------------------------------------------------------
void HttpManager::Reconnect()
{
	SocketConnect(mSocket,mHost,mPort);
}

//------------------------------------------------------------------------------------------------
void HttpManager::Disconnect()
{
	SocketClose(mSocket);
}

//------------------------------------------------------------------------------------------------
void HttpManager::SendRequest(char* page, char *data, int type)
{
	bool exists = false;
	for (int i=0; i<mRequests.size(); i++) {
		if (mRequests[i].type == type
			&& strcmp(mRequests[i].page, page) == 0
			&& strcmp(mRequests[i].data, data) == 0) {

			exists = true;
			break;
		}
	}

	if (!exists) {
		Request req = Request();
		strcpy(req.page, page);
		strcpy(req.data, data);
		req.type = type;
		mRequests.push_back(req);
		//Reconnect();
	}
	Reconnect();
}

//------------------------------------------------------------------------------------------------
int HttpManager::GetResponse(char* buffer)
{
	//char* buffer = NULL;
	if (mResponses.size() > 0) {
		Response response = mResponses.front();
		memcpy(buffer,response.buffer,response.size);
		buffer[response.size] = '\0';
		//strcpy(buffer,response.buffer);
		//buffer = mResponses.front();
		mResponses.pop_front();
		return response.size;
	}

	return -1;
}

//------------------------------------------------------------------------------------------------
int HttpManager::GetContentSize()
{
	int temp = mContentSize;
	mContentSize = -1; //reset
	return temp;
}

//------------------------------------------------------------------------------------------------
bool HttpManager::ResponseReady()
{
	if (mResponses.size() > 0) {
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------
void HttpManager::ClearRequests()
{
	while (mRequests.size() > 0) {
		//delete mRequests.front();
		mRequests.pop_front();
	}
	//mRequests.clear();
}

//------------------------------------------------------------------------------------------------
void HttpManager::Update(float dt)
{
	mTimer += dt;
	mReconnectTimer += dt;

	if (mReconnectTimer > 2000) {
		//Connect(mHost,mPort);
	}
	if (mTimer > 250) {
		if (mRequests.size() > 0) {
			Request req = mRequests.front();

			char buffer[4096];
			if (req.type == REQUEST_GET) {
				sprintf(buffer,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",req.page,mHostText);
			}
			else if (req.type == REQUEST_POST) {
				sprintf(buffer,"\
POST %s HTTP/1.1\r\n\
Host: %s\r\n\
Content-Length: %d\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
\r\n\
%s\r\n\r\n",req.page,mHostText,strlen(req.data),req.data);
			}

			int n = SocketSend(mSocket,buffer,strlen(buffer));
			if (n > 0) {
				//delete mRequests.front();
				mRequests.pop_front();
				mReconnectTimer = 0.0f;
			}
		}
		mTimer = 0.0f;
	}

		char buffer[8192];
		int n = SocketRecv(mSocket,buffer,8191);
		if (n > 0) {
			mReconnectTimer = 0.0f;
			buffer[n] = '\0';
			char* s = buffer;
			while (s != NULL) {
				if (ReadHTTP(s,n)) {
					Response response;
					int copy = mBufferSize;
					if (copy < 0) copy = 0;
					if (copy > (int)sizeof(response.buffer) - 1) copy = (int)sizeof(response.buffer) - 1;
					memcpy(response.buffer,mBuffer,copy);
					response.size = copy;
					mResponses.push_back(response);

					strcpy(mBuffer,"");
					mBufferSize = 0;
					mContentType = CONTENT_UNKNOWN;
				}
				s = strstr(s,"\r\n0\r\n\r\nHTTP");
				if (s == NULL) break;
				s += 7;
			}
		}
}


//------------------------------------------------------------------------------------------------
bool HttpManager::ReadHTTP(char* string, int n) {
	char* s;
	char* end = string + n;
	int cap = (int)sizeof(mBuffer);
	if (strstr(string,"chunked")) {
		mContentType = CONTENT_CHUNKED;
		s = strstr(string,"\r\n\r\n");
		if (s == NULL) return false;
		s += 4;
	}
	else if (strstr(string,"Server: dbws")) {
		mContentType = CONTENT_NORMAL;

		s = strstr(string,"content-length: ");
		if (s == NULL) {
			return false;
		}
		s += strlen("content-length: ");

		std::string numberstring = "";
		while (s < end && *s != '\x0d') {
			numberstring += *s;
			s += 1;
		}
		mContentSize = atoi(numberstring.c_str());

		s = strstr(string,"\r\n\r\n");
		if (s == NULL) return false;
		s += 4;
	}
	else {
		//mContentType = CONTENT_UNKNOWN;
		s = string;
	}


	if (mContentType == CONTENT_CHUNKED) {
		while (s < end && *s != '\0') {
			int length = strtol(s,&s,16);
			if (length <= 0) {
				return true;
			}
			if (s + 2 > end) break;
			s += 2; // \r\n

			for (int i=0; i<length; i++) {
				if (s >= end) break;
				if (mBufferSize >= cap - 1) break;
				mBuffer[mBufferSize++] = *s;
				s++;
			}
			if (mBufferSize >= cap - 1) break;
		}
	}
	else if (mContentType == CONTENT_NORMAL) {
		int size = n-(int)(s-string);
		if (size < 0) size = 0;
		if (mBufferSize + size > cap - 1) size = (cap - 1) - mBufferSize;
		if (size < 0) size = 0;
		memcpy(mBuffer + mBufferSize,&s[0],size);
		mBufferSize += size;
		return true;
	}
	else {
		int size = n-(int)(s-string);
		if (size < 0) size = 0;
		if (mBufferSize + size > cap - 1) size = (cap - 1) - mBufferSize;
		if (size < 0) size = 0;
		memcpy(mBuffer + mBufferSize,&s[0],size);
		mBufferSize += size;
		return true;
	}

	return false;
}