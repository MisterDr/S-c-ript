#include "bstrlib.h"
#include <stdio.h>

int main(int argc, char* argv) {
    bstring msg = bfromcstr("This is a message with some \" embeds \" 2 \" 3");
    bfindreplace(msg, bfromcstr("\""), bfromcstr("'"), 0);
    printf("%s\n", msg->data);
    return 0;
}