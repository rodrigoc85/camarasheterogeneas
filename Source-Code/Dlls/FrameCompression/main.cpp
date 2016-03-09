#include "main.h"

std::vector< unsigned char > DLL_EXPORT frame_compress(const std::vector< unsigned char > & src) {
    std::vector< unsigned char > ret;

	uLongf ret_size = ::compressBound( src.size() );
	ret.resize( ret_size + sizeof( uLongf ) );
    int level = 3;
	if ( level == 0 )
	{
		int error = ::compress( &ret[0] + sizeof( uLongf ), &ret_size, &src[0], src.size() );
		if ( error == Z_OK )
		{
			ret.resize( ret_size + sizeof( uLongf ), 0 );
		}
		else
		{
			std::cout << "ofxZip::compress()" << "zlib compress() error: " << error;
			ret.clear();
		}
	}
	else
	{
		int error = compress2( &ret[0] + sizeof( uLongf ), &ret_size, &src[0], src.size(), level );
		if ( error == Z_OK )
		{
			ret.resize( ret_size + sizeof( uLongf ), 0 );
		}
		else
		{
			std::cout << "ofxZip::compress()" << "zlib compress2() error: " << error;
			ret.clear();
		}
	}

	/// push header (size of compressed buffer)
	{
		uLongf originalSize = src.size();
		ret[0] = ( originalSize >> 24 ) & 0xFF;
		ret[1] = ( originalSize >> 16 ) & 0xFF;
		ret[2] = ( originalSize >>  8 ) & 0xFF;
		ret[3] = ( originalSize >>  0 ) & 0xFF;

	}

	return ret;
}

std::vector< unsigned char > DLL_EXPORT frame_uncompress( const std::vector< unsigned char > & src ) {
    std::vector< unsigned char > ret;

	if ( src.size() < 4 ) return ret;

	/// load header (size of compressed buffer)
	uLongf originalSize = 0;
	{
		originalSize += ( src[0] << 24 );
		originalSize += ( src[1] << 16 );
		originalSize += ( src[2] <<  8 );
		originalSize += ( src[3] <<  0 );

	}
	//cout << originalSize << endl;

	ret.resize( originalSize, 0 );
	unsigned long ret_size = originalSize;

	{
		int error = ::uncompress( &ret[0], &ret_size, &src[0] + sizeof( uLongf ), src.size() );
		if ( error == Z_OK )
		{
			ret.resize( ret_size, 0 );
		}
		else
		{
			std::cout << "ofxZip::uncompress()" << "zlib uncompress() error: " << error;
			ret.clear();
		}
	}

	return ret;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
