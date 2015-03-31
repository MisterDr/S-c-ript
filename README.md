# S-c-ript
S(c)ript Tiny C Web Engine

Here is the simple usage for S(c)ript rendering engine:

```html
<html lang="en-US">
<head>
</head>
<body>
	<$ 
		#include <stdio.h>
		#include <headers.h>

		void main(int argc, char **argv)
		{
			TCC_HEADER_HTML
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
```