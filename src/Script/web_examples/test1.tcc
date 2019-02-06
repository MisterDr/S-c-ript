<$	#include <stdio.h> 	
#include <headers.h>
void main(int argc, char **argv) {
TCC_HEADER_HTML 
int a = 1;$>
<div>
  <$ if (a == 1) {
$><a href=/\">aaa</a>
<$ } else { $>
<a href=\"/\">bbb</a>
<$ } $>
<p></p>
<$ } $>
</div>