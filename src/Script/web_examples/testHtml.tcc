<html lang="en-US">
<head>
</head>
<body>
	<$ 
		#include <stdio.h>
		#include <headers.h>

		void main(int argc, char **argv)
		{
			HTML
			printf("Hello World\n");			
		
	$>
	
	<div></div>
	
	<$
			printf("Hello2");
			return 0;
		}
	$>
</body>
</html>

