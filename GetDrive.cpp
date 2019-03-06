
#ifndef GET_DRIVE_CPP
#define GET_DRIVE_CPP

#include <windows.h>
#include <winioctl.h>
#include <cstdio>

class Drive
{
public:
	
	HANDLE drv;
	
	unsigned long long bytes;
	unsigned long long sectors;
	unsigned long long sectorSize;
	
	unsigned long long cylinders;
	unsigned long long tracksPerCylinder;
	unsigned long long SectorsPerTrack;
	
public:
	
	bool IsValid()
	{
		return this->drv != INVALID_HANDLE_VALUE;
	}
	
	void WriteSector( void * buffer, unsigned long long sectorID )
	{
		if( this->IsValid() )
		{
			if( sectorID < this->sectors )
			{
				DWORD dwWrite;
				SetFilePointer( this->drv, this->sectorSize * sectorID, 0, FILE_BEGIN );
				WriteFile( this->drv, buffer, this->sectorSize, &dwWrite, NULL );
			}
		}
	}
	
	void ReadSector( void * buffer, unsigned long long sectorID )
	{
		if( this->IsValid() )
		{
			if( sectorID < this->sectors )
			{
				DWORD dwRead;
				SetFilePointer( this->drv, this->sectorSize * sectorID, 0, FILE_BEGIN );
				ReadFile( this->drv, buffer, this->sectorSize, &dwRead, NULL );
			}
		}
	}
	
	void PrintSize()
	{
		if( this->IsValid() )
		{
			printf( "\n Sectors:     %lli", this->sectors );
			printf( "\n Sector size: %lli", this->sectorSize );
			printf( "\n Size:        %lliB", this->bytes );
			printf( "\n              %0.2fK", float(this->bytes)/1024.0f );
			printf( "\n              %0.2fM", float(this->bytes)/(1024.0f*1024.0f) );
			printf( "\n              %0.2fG", float(this->bytes)/(1024.0f*1024.0f*1024.0f) );
		}
		else
		{
			printf( "\n Invalid drive" );
		}
	}
	
	int Open( const char * driveString, bool write = false )
	{
		this->Close();
		
		this->drv = CreateFile( driveString,
								GENERIC_READ | ( write ? GENERIC_WRITE : 0 ),
								FILE_SHARE_READ | ( write ? FILE_SHARE_WRITE : 0 ), 
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, // | FILE_FLAG_NO_BUFFERING,
								NULL );
		
		if( this->drv != INVALID_HANDLE_VALUE )
		{
			DISK_GEOMETRY pdg;
			DWORD junk = 0;
			
			DeviceIoControl(    this->drv,
								IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL, 0,
								&pdg, sizeof(pdg),
								&junk,
								(LPOVERLAPPED)NULL );
			
			this->bytes = (unsigned long long)pdg.Cylinders.QuadPart * (unsigned long long)pdg.TracksPerCylinder * (unsigned long long)pdg.SectorsPerTrack * (unsigned long long)pdg.BytesPerSector;
			this->sectors = (unsigned long long)pdg.Cylinders.QuadPart * (unsigned long long)pdg.TracksPerCylinder * (unsigned long long)pdg.SectorsPerTrack;
			this->sectorSize = (unsigned long long)pdg.BytesPerSector;
			
			this->cylinders = (unsigned long long)pdg.Cylinders.QuadPart;
			this->tracksPerCylinder = (unsigned long long)pdg.TracksPerCylinder;
			this->SectorsPerTrack = (unsigned long long)pdg.SectorsPerTrack;
			
			return 0;
		}
		
		return 1;
	}
	
	void Close()
	{
		if( this->drv != INVALID_HANDLE_VALUE )
			CloseHandle( this->drv );
		this->drv = INVALID_HANDLE_VALUE;
	}
	
	Drive()
	{
		this->drv = INVALID_HANDLE_VALUE;
	}
	
	~Drive()
	{
		this->Close();
	}
};

#endif

