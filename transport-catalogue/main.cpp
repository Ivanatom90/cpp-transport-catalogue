#include <iostream>
#include <fstream>

#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std;

int main()
{
    RequestBase B = GetRequestBase(cin);
    TransportCatalog TK(B);
    RequestOut RB(TK, cin);
    return 0;
}
