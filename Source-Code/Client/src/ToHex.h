const char * HexEncode(unsigned char *c) {
    std::string tmp;
    char buf[3];

    while(c != 0) {
        sprintf(buf, "%02X", c);
        tmp += buf;
        c++;
    }

    return tmp.c_str();
}

const char * HexDecode(const char *c) {
    std::string tmp;
    unsigned int x;

    while(*c != 0) {
        sscanf(c, "%2X", &x);
        tmp += x;
        c += 2;
    }

    return tmp.c_str();
}
