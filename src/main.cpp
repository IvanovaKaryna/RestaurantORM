#include <iostream>
#include "Demos.h"
#include "TestData.h"

int main()
{
    try
    {
        runCompareDemo();
        runRepositoryDemo();

        prepareTestData();
        runRelationDemo();
        runQueryDemo();
        runAggregateDemo();
        runJoinDemo();
        runTransactionDemo();
        runRollbackDemo();
        runWithoutTransactionDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}