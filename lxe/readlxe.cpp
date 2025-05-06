#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

enum lhtype {
	TINVAL,
	LIX_8_16,
};

struct lxe_hdr {
	short lh_magic[3];
	short lh_type;
	short lh_sec_code_begin;
	short lh_sec_data_begin;
	short lh_sec_symtab_begin;
	short lh_symtb_entries;
} __attribute__((packed));

struct lix_exe_symtb_entry {
	short name[256];
	short null_byte;
	short symbegin;
} __attribute__((packed));

using namespace std;

// When im 18 my parents said i can reveal my full name
#define COPYRIGHT                                                                                                      \
	"Alexander"                                                                                                    \
	"LowLevelCodingCH"

lxe_hdr read_header(int fd)
{
	lxe_hdr hdr = {0};

	(void) read(fd, (void *) &hdr, sizeof(lxe_hdr));

	return hdr;
}

vector<lix_exe_symtb_entry> read_symtb_entries(int fd, lxe_hdr &hdr)
{
	vector<lix_exe_symtb_entry> entries;
	entries.resize(hdr.lh_symtb_entries);

	struct stat status;

	(void) fstat(fd, &status);

	string symtb;
	symtb.reserve(status.st_size);
	char *dat = (char *) symtb.data();

	(void) read(fd, (void *) dat, status.st_size);

	dat += (hdr.lh_sec_symtab_begin * 2) - sizeof(lxe_hdr);

	for (int i = 0; i < hdr.lh_symtb_entries; ++i) {
		memcpy((void *) &entries[i], (void *) dat, sizeof(lix_exe_symtb_entry));
		dat += sizeof(lix_exe_symtb_entry);
	}

	return entries;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		cerr << "Usage: readlxe <FILE>" << endl;
		abort();
	}

	int fd = open(argv[1], O_RDONLY);

	if (fd == -1) {
		cerr << "Unable to open file " << string(argv[1]) << endl;
		abort();
	}

	lxe_hdr hdr = read_header(fd);

	vector<lix_exe_symtb_entry> entries = read_symtb_entries(fd, hdr);

	wcout << "Magic                 " << (wchar_t) hdr.lh_magic[0] << (wchar_t) hdr.lh_magic[1]
	      << (wchar_t) hdr.lh_magic[2] << endl;
	cout << "Type                  " << hdr.lh_type << endl;
	cout << "Section .code begin   " << hdr.lh_sec_code_begin << endl;
	cout << "Section .data begin   " << hdr.lh_sec_data_begin << endl;
	cout << "Section .symtab begin " << hdr.lh_sec_symtab_begin << endl;
	cout << "Symtab Entry Amount   " << hdr.lh_symtb_entries << endl;
	cout << endl;
	cout << "Symtab entries" << endl;

	for (const auto &entry : entries) {
		int i = 0;
		while (entry.name[i]) {
			wcout << (wchar_t) entry.name[i];
			++i;
		}
		cout << endl << "                  " << entry.symbegin << endl;
	}

	(void) close(fd);

	return 0;
}
