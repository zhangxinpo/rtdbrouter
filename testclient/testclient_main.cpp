#include "rtdbrouterclient.h"
#include <iostream>
#include <zmq.h>
#include <windows.h>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QString>
#include <QList>

struct TestData
{
	char szFam[16];
	char szApp[16];
	char szDB[16];
	char szTable[16];
	char szField[16];
	char szFieldType[16];
	int nFieldSize;
	int nRow;
};

int main(int argc, char* argv[])
{
	bool bInitialized = Initialize();
	if ( bInitialized == false )
	{
		std::cout << "Initialize return false!" << std::endl;
		return -1;
	}

	//int hSubscription = CreateSubscriptionHandle();
	//if ( hSubscription < 0 )
	//{
	//	std::cout << "CreateSubscriptionHandle return " << hSubscription << " !" << std::endl;
	//	return -1;
	//}

	//读配置文件，增加订阅数据
	QDomDocument doc("mydocument");
	QFile file("./testrtdbrouter.xml");
	if (!file.open(QIODevice::ReadOnly))
	{
		std::cout << " Open testrtdbrouter.xml failed! "<< std::endl;
		return -1;
	}

	if (!doc.setContent(&file)) 
	{
		std::cout << " setContent testrtdbrouter.xml failed! "<< std::endl;

		file.close();
		return -1;
	}
	file.close();

	// print out the element names of all elements that are direct children
	// of the outermost element.
	QDomElement root = doc.documentElement();
    if (root.tagName() != "TESTRTDBROUTER") 
	{
		std::cout << "The file is not an TESTRTDBROUTER file" << std::endl;
        return -1;
    } 

	QList<TestData*> lstTestData;

	QDomNode n = root.firstChild();
	while(!n.isNull()) 
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull()) 
		{
			QString strFam = e.attribute("FAM");
			QString strApp = e.attribute("APP");
			QString strDB = e.attribute("DB");
			QString strTable = e.attribute("TABLE");
			QString strField = e.attribute("FIELD");
			QString strFieldType = e.attribute("FIELDTYPE");
			QString strFieldSize = e.attribute("FIELDSIZE");
			QString strRow = e.attribute("ROW");

			TestData* testData = new TestData;
			strcpy(testData->szFam, strFam.toLatin1().constData() );
			strcpy(testData->szApp, strApp.toLatin1().constData() );
			strcpy(testData->szDB, strDB.toLatin1().constData() );
			strcpy(testData->szTable, strTable.toLatin1().constData() );
			strcpy(testData->szField, strField.toLatin1().constData() );
			strcpy(testData->szFieldType, strFieldType.toLatin1().constData() );
			testData->nFieldSize = strFieldSize.toInt();
			testData->nRow = strRow.toInt();
			lstTestData.append(testData);
			//AddSubRTDBData(hSubscription, testData->szFam, 
			//	testData->szApp,
			//	testData->szDB,
			//	testData->szTable,
			//	testData->szField,
			//	testData->nRow);
		}
		n = n.nextSibling();
	}

	//bool bRegistered = RegisterSubscript(hSubscription);
	//if ( bRegistered == false )
	//{
	//	std::cout << "RegisterSubscript return false!" << std::endl;
	//	return -1;
	//}

	int nCount = 1000;
	while (nCount-- > 0)
	{
		//bool bRefresh = RefreshSubscript(hSubscription);
		//if ( bRefresh == false )
		//{
		//	std::cout << "RefreshSubscript return false!" << std::endl;
		//	return -1;
		//}
		int nReturnCode = 0;
		double dValue = 0;
		int nValue = 0;
		char szBuffer[1024];
		for (int i = 0; i < lstTestData.size(); ++i) 
		{
			TestData* pTestData = lstTestData.at(i);
			if ( strcmp(pTestData->szFieldType, "GDB_DT_INT" ) == 0 && pTestData->nFieldSize == 4 )
			{
				nReturnCode = ReadFieldInt4(pTestData->szFam, pTestData->szApp, pTestData->szDB, pTestData->szTable, pTestData->szField, pTestData->nRow, &nValue);
				if ( nReturnCode >= 0 )
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << nValue << std::endl;
				}
				else
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << "return error" << std::endl;
				}
			}

			if ( strcmp(pTestData->szFieldType, "GDB_DT_FLOAT" ) == 0 && pTestData->nFieldSize == 8 )
			{
				nReturnCode = ReadFieldDouble(pTestData->szFam, pTestData->szApp, pTestData->szDB, pTestData->szTable, pTestData->szField, pTestData->nRow, &dValue);
				if ( nReturnCode >= 0 )
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << dValue << std::endl;
				}
				else
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << "return error" << std::endl;
				}
			}

			if ( strcmp(pTestData->szFieldType, "GDB_DT_CHAR" ) == 0 && pTestData->nFieldSize < 1024 )
			{
				nReturnCode = ReadFieldString(pTestData->szFam, pTestData->szApp, pTestData->szDB, pTestData->szTable, pTestData->szField, pTestData->nRow, szBuffer, sizeof(szBuffer));
				if ( nReturnCode >= 0 )
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << szBuffer << std::endl;
				}
				else
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << "return error" << std::endl;
				}
			}
			if ( strcmp(pTestData->szFieldType, "GDB_DT_TIME" ) == 0 && pTestData->nFieldSize == 8 )
			{
				nReturnCode = ReadFieldDouble(pTestData->szFam, pTestData->szApp, pTestData->szDB, pTestData->szTable, pTestData->szField, pTestData->nRow, &dValue);
				if ( nReturnCode >= 0 )
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << dValue << std::endl;
				}
				else
				{
					std::cout << pTestData->szFam << " " <<pTestData->szApp << " " << pTestData->szDB << " " << pTestData->szTable 
						<< " " << pTestData->szField << " " << pTestData->nRow <<" " << "return error" << std::endl;
				}
			}
		}


		Sleep(1000);
	}


	//bool bCancelSubscript = CancelSubscript(hSubscription);
	//if ( bCancelSubscript == false )
	//{
	//	std::cout << "CancelSubscript return false!" << std::endl;
	//	return -1;
	//}

	bool bDeinitialize = Deinitialize();
	if ( bDeinitialize == false )
	{
		std::cout << "Deinitialize return false!" << std::endl;
		return -1;
	}

	return 0;
}