
#include "GetDrive.cpp"

#include <ctime>

int main( int argc, char ** argv )
{
	if( argc >= 3 )
	{
		FILE * file = fopen( argv[2], "wb" );
		
		if( file )
		{
			Drive * drv = new Drive();
			
			drv->Open( argv[1], false );
			
			if( drv->IsValid() )
			{
				drv->PrintSize();
				
				char * buffer = new char[drv->sectorSize*128];
				memset( buffer, 0 , drv->sectorSize*128 );
				
				unsigned long long bytesReaded = 0, bytesToWrite = 0;
				unsigned tb = clock();
				unsigned t = clock();
				unsigned long long i;
				
				for( i = 0; i < drv->sectors; ++i )
				{
					/*
					if( clock() - t > 8192 || i+1 == drv->sectors )
					{
						printf( "\n Reading %lli/%lli (%lli/%lli B)  with speed: %.2f B/s", i+1, drv->sectors, (i+1)*drv->sectorSize, drv->bytes, float(bytesReaded)*1000.0f/float(clock()-t) );
						bytesReaded = 0;
						t = clock();
					}
					memset( buffer, 0 , drv->sectorSize );
					*/
					drv->ReadSector( buffer+((i%128)*drv->sectorSize), i );
					bytesReaded += drv->sectorSize;
					
					bytesToWrite += drv->sectorSize;
					if( i != 0 && i % 128 == 0 )
					{
						printf( "\n Reading %lli/%lli (%lli/%lli B)  with speed: %.2f B/s", i+1, drv->sectors, (i+1)*drv->sectorSize, drv->bytes, float(bytesReaded)*1000.0f/float(clock()-t) );
						fwrite( buffer, 1, bytesToWrite, file );
						memset( buffer, 0 , drv->sectorSize*128 );
						bytesToWrite = 0;
						bytesReaded = 0;
						t = clock();
					}
				}
				
				if( bytesToWrite != 0 )
					fwrite( buffer, 1, bytesToWrite, file );
				
				printf( "\n Done in: %.2f s, average speed: %.2f B/s", float(clock()-tb)*0.001f, float(drv->bytes)*1000.0f/float(clock()-tb) );
			}
			else
			{
				printf( "\n Cannot acces drive" );
			}
			
			fclose( file );
			delete drv;
		}
		else
		{
			printf( "\n Cannot open file to write disk data" );
		}
	}
	else
	{
		printf( "\n No file specified to write or no drive specified to read" );
	}
	
	return 0;
}



