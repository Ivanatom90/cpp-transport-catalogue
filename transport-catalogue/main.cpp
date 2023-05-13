#include <iostream>
#include <fstream>

#include "input_reader.h"
//#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std;

int main()
{
    RequestBase B(cin);
    RequestOut RB(B.TCatalog_, cin);
    RB.GetOutPut();
    return 0;
}

