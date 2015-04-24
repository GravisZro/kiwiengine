#include "file.h"

bool CFileResponse::requestFilter(CFileRequestEvent* request)
{

  QTextStream os(reinterpret_cast<QIODevice*>(request->socket()));
  os.setAutoDetectUnicode(true);

  os << "HTTP/1.1 200 Ok\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "\r\n";

  os << "<html>"
        "<head><title>"
     << request->fileName()
     << "</title></head><body>"
     << "<pre>"
     << "\nText:\r\n"
     << "FILE PAGE :/"
     << "</pre>"
     << "</body></html>\r\n";

  return false;
}
