function hexnumber(str,  ret, n, k)
{
    n = length(str)
    ret = 0
    for (i = 1; i <= n; i++) {
        c = substr(str, i, 1)
        c = tolower(c)
        # index() returns 0 if c not in string,
        # includes c == "0"
        k = index("123456789abcdef", c)

        ret = ret * 16 + k
    }
    return ret;
}

function octnumber(str,  ret, n, k)
{
    n = length(str)
    ret = 0
    for (i = 1; i <= n; i++) {
        c = substr(str, i, 1)
        # index() returns 0 if c not in string,
        # includes c == "0"
        k = index("1234567", c)

        ret = ret * 8 + k
    }
    return ret;
}


BEGIN { RS="\\" ; notQuoted = 0}
#{ printf("'%s'\n",$0) > "/dev/stderr" ; next}
NR == 1 {
    if(match($0,/^"/)) {
        split($0,matches,"\"")
        printf("%s",matches[2]);
        if(match($0,/^"[^"]*"/)) {
            exit 0;
        }
        next
    } else {
        exit 1;
    }
}

#This will happen on the 2nd (and subsequent) consecutive backslash
/^$/ {
    if (notQuoted) {
        notQuoted = 0
    } else {
    notQuoted = 1
    printf("\\")
    next
    }
}
notQuoted == 1 {
    if(match($0,/([^"]*)"/))
    {
        split($0,matches,"\"")
        printf("%s",matches[1])
        exit 0
    }
    notQuoted = 0;
    printf("%s",$0);
    next
}
# If we reach here, then the first character is part of a backslash escape
{
    first = substr($0,1,1);
    rest = substr($0,2);

    if(first == "a") printf("\a")
    else if(first == "b") printf("\b")
        else if(first == "f") printf("\f")
        else if(first == "n") printf("\n")
        else if(first == "r") printf("\r")
        else if(first == "t") printf("\t")
        else if(first == "v") printf("\v")
        else if(first == "\"") printf("\"")
        else if (first == "x") {
            second = substr(rest,1,1);
            third = substr(rest,2,1);
            if(match(third,/[1234567890ABCDEFabcdef]/)) {
                printf("%c",hexnumber(second third)+0);
                rest = substr(rest,3);
            }
            else {
                printf("%c",hexnumber(second)+0);
                rest = substr(rest,2);
            }
        }
        else if (match(first,/[01234567]/)) {
            second = substr(rest,1,1);
            third = substr(rest,2,1);
            fourth = substr(rest,3,1);
            if(match(second third fourth,/^[01234567]*$/)) {
                printf("%c",octnumber("0" first second third fourth)+0);
                rest = substr(rest,length(first second third fourth));
            }
            else if(match(second third,/^[01234567]*$/)) {
                printf("%c",octnumber(first second third)+0);
                rest = substr(rest,length(first second third));
            }
            else if(match(second,/^[01234567]*$/)) {
                printf("%c",octnumber(first second)+0);
                rest = substr(rest,length(first second));
            }
            else {
                printf("%c",octnumber(first)+0);
            }
        }

    if(match(rest,/([^"]*)"/))  # this comment fixes vim's syntax matching " 
    {
        split(rest,matches,"\"")
        printf("%s",matches[1])
        exit 0
    }

    printf("%s",rest);
}

