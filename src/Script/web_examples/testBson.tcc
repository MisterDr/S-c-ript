<html lang="en-US">
<head>
</head>
<body>
	<$ 
		#define _WIN32;
		#include <stdio.h>
		#include <headers.h>		
		#include <bson.h>
		#include <string.h>
		#include <stdlib.h>
		#include <bson.c>
		#include <encoding.c>
		#include <numbers.c>
		
		#include <md5.c>
		#include <env_default.c>
		
		#include <mongo.c>
		
		//Header must be the first statement
		void main(int argc, char **argv)
		{			
			TCC_HEADER_JSON
			
			printf("Hello World\n");			
			
			bson b, sub;
			bson_iterator it;
			
			    bson_init( &b );
				bson_append_new_oid( &b, "_id" );
				bson_append_new_oid( &b, "user_id" );

				bson_append_start_array( &b, "items" );
				bson_append_start_object( &b, "0" );
				bson_append_string( &b, "name", "John Coltrane: Impressions" );
				bson_append_int( &b, "price", 1099 );
				bson_append_finish_object( &b );

				bson_append_start_object( &b, "1" );
				bson_append_string( &b, "name", "Larry Young: Unitys" );
				bson_append_int( &b, "price", 1199 );
				bson_append_finish_object( &b );
				bson_append_finish_object( &b );

				bson_append_start_object( &b, "address" );
				bson_append_string( &b, "street", "59 18th St." );
				bson_append_int( &b, "zip", 10010 );
				bson_append_finish_object( &b );

				bson_append_int( &b, "total", 2298 );

				bson_finish( &b );

				/* Advance to the 'items' array */
				bson_find( &it, &b, "items" );

				/* Get the subobject representing items */
				bson_iterator_subobject( &it, &sub );

				/* Now iterate that object */
				bson_print( &sub );					
		
	$>
	
	<div></div>
	
	<$
			printf("Hello2s");		
		}
	$>
</body>
</html>
