const MAXNUMBERS = 65535;

struct request {
    int numbers<MAXNUMBERS>;
};

union response switch (bool error) {
    case 0:
        int suma;
    case 1:
        string errmsg<3>;
};