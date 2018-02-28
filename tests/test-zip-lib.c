// testeZip.cpp : define o ponto de entrada para o aplicativo do console.
//
#include "../nclua/event/zip.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZIPNAME "test.zip"
#define TESTDATA1 "Some test data 1...\0"
#define TESTDATA2 "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ullamcorper sit amet risus nullam eget. Eget mi proin sed libero. Tristique senectus et netus et malesuada fames. Amet mauris commodo quis imperdiet massa tincidunt nunc pulvinar. Vitae elementum curabitur vitae nunc sed velit dignissim. A cras semper auctor neque vitae tempus quam. Velit sed ullamcorper morbi tincidunt ornare massa. Tempor commodo ullamcorper a lacus. Et tortor consequat id porta. Purus viverra accumsan in nisl nisi scelerisque eu. Nunc lobortis mattis aliquam faucibus purus. \
\
Id donec ultrices tincidunt arcu non sodales neque sodales.Orci nulla pellentesque dignissim enim sit amet venenatis.Tellus mauris a diam maecenas sed enim ut sem viverra.Eleifend donec pretium vulputate sapien nec sagittis aliquam malesuada.Aenean et tortor at risus viverra adipiscing.Mauris cursus mattis molestie a iaculis at erat.Arcu non sodales neque sodales ut etiam.Semper auctor neque vitae tempus quam pellentesque.Sed turpis tincidunt id aliquet risus.Nunc mi ipsum faucibus vitae.Turpis cursus in hac habitasse platea.Arcu vitae elementum curabitur vitae.Condimentum mattis pellentesque id nibh.Turpis cursus in hac habitasse platea dictumst.Viverra justo nec ultrices dui sapien eget mi proin sed. \
\
\
Dignissim convallis aenean et tortor at risus viverra adipiscing at.Mi tempus imperdiet nulla malesuada pellentesque elit eget gravida.Mattis pellentesque id nibh tortor id.Ut venenatis tellus in metus.Volutpat maecenas volutpat blandit aliquam etiam erat velit scelerisque.Ornare arcu dui vivamus arcu felis.Nulla facilisi cras fermentum odio eu feugiat pretium nibh ipsum.Sit amet mauris commodo quis imperdiet massa tincidunt nunc.Vulputate ut pharetra sit amet aliquam id.At augue eget arcu dictum varius duis at consectetur.Aliquam sem fringilla ut morbi tincidunt augue interdum.Aliquam vestibulum morbi blandit cursus risus at ultrices mi tempus.\
\
Faucibus turpis in eu mi bibendum neque.Velit egestas dui id ornare.Odio eu feugiat pretium nibh.Facilisis leo vel fringilla est ullamcorper eget nulla facilisi.Egestas egestas fringilla phasellus faucibus scelerisque eleifend donec pretium vulputate.Netus et malesuada fames ac turpis.Sapien pellentesque habitant morbi tristique.Magna sit amet purus gravida quis blandit turpis cursus in.Morbi tempus iaculis urna id volutpat lacus laoreet non curabitur.Morbi tincidunt augue interdum velit euismod in pellentesque.Consectetur lorem donec massa sapien faucibus et molestie.Fermentum et sollicitudin ac orci phasellus.Vel pretium lectus quam id leo in.Nisl nisi scelerisque eu ultrices.Viverra mauris in aliquam sem.Scelerisque eleifend donec pretium vulputate sapien nec sagittis aliquam malesuada.Feugiat nibh sed pulvinar proin gravida hendrerit lectus a.Non odio euismod lacinia at quis.Nulla pellentesque dignissim enim sit amet venenatis urna cursus eget.\
\
Tempor nec feugiat nisl pretium fusce id velit ut.Ornare suspendisse sed nisi lacus sed viverra tellus in.Odio tempor orci dapibus ultrices in iaculis nunc sed augue.Leo urna molestie at elementum eu facilisis.Quis vel eros donec ac.In hac habitasse platea dictumst quisque sagittis.Facilisi nullam vehicula ipsum a arcu cursus vitae.Sapien eget mi proin sed libero enim sed faucibus turpis.Eget lorem dolor sed viverra ipsum nunc aliquet.Amet massa vitae tortor condimentum.Arcu dui vivamus arcu felis bibendum ut tristique et egestas.Justo laoreet sit amet cursus sit amet dictum sit amet.\
\
Commodo quis imperdiet massa tincidunt nunc pulvinar sapien et.Quisque sagittis purus sit amet.Augue mauris augue neque gravida in fermentum et.Nunc pulvinar sapien et ligula ullamcorper malesuada proin libero.Congue mauris rhoncus aenean vel elit scelerisque mauris.Quam quisque id diam vel quam.Nunc congue nisi vitae suscipit.Tristique sollicitudin nibh sit amet commodo nulla.Nulla facilisi morbi tempus iaculis.Turpis massa tincidunt dui ut ornare.Neque aliquam vestibulum morbi blandit cursus.Ullamcorper a lacus vestibulum sed arcu non odio euismod.Sem viverra aliquet eget sit amet tellus cras adipiscing.Dui vivamus arcu felis bibendum ut tristique.Ipsum faucibus vitae aliquet nec ullamcorper sit amet risus nullam.Vel pharetra vel turpis nunc eget lorem dolor sed.Purus gravida quis blandit turpis cursus.\
\
Varius morbi enim nunc faucibus a.Orci nulla pellentesque dignissim enim sit amet.Libero nunc consequat interdum varius.Adipiscing elit ut aliquam purus.Neque sodales ut etiam sit amet nisl purus.Quis commodo odio aenean sed.Eu mi bibendum neque egestas congue quisque egestas diam.Eget felis eget nunc lobortis.Tristique sollicitudin nibh sit amet commodo nulla facilisi nullam vehicula.Eget nulla facilisi etiam dignissim diam quis.Consectetur adipiscing elit duis tristique sollicitudin nibh sit amet commodo.Lorem ipsum dolor sit amet consectetur adipiscing elit.Euismod nisi porta lorem mollis aliquam ut porttitor leo.Ullamcorper morbi tincidunt ornare massa eget egestas purus viverra accumsan.Sed risus ultricies tristique nulla aliquet enim.Lacus viverra vitae congue eu consequat.Ac auctor augue mauris augue neque gravida in fermentum.Lacus luctus accumsan tortor posuere ac ut.Quis eleifend quam adipiscing vitae.\
\
Purus sit amet volutpat consequat mauris nunc.Vestibulum mattis ullamcorper velit sed.Quam adipiscing vitae proin sagittis nisl rhoncus mattis.Tristique sollicitudin nibh sit amet.Placerat in egestas erat imperdiet sed.Molestie at elementum eu facilisis sed odio morbi quis.Sit amet venenatis urna cursus eget nunc scelerisque viverra mauris.Ut etiam sit amet nisl purus in mollis nunc.Iaculis nunc sed augue lacus viverra.Diam phasellus vestibulum lorem sed risus ultricies tristique nulla aliquet.Tellus cras adipiscing enim eu turpis egestas pretium aenean pharetra.In massa tempor nec feugiat.\
\
Scelerisque fermentum dui faucibus in ornare quam viverra orci.Massa id neque aliquam vestibulum.Pharetra pharetra massa massa ultricies mi quis hendrerit.Iaculis eu non diam phasellus vestibulum lorem.Leo a diam sollicitudin tempor id eu nisl nunc mi.Aliquam ut porttitor leo a diam sollicitudin tempor id.Lectus sit amet est placerat in egestas erat.Placerat duis ultricies lacus sed.Quis viverra nibh cras pulvinar mattis nunc.Mollis aliquam ut porttitor leo a diam sollicitudin tempor id.Mauris augue neque gravida in fermentum et.Dictum non consectetur a erat nam at.Tortor pretium viverra suspendisse potenti nullam ac tortor.Sed lectus vestibulum mattis ullamcorper velit sed ullamcorper.\
\
Ac tortor dignissim convallis aenean et tortor at.Cras semper auctor neque vitae tempus quam pellentesque.Tristique senectus et netus et malesuada fames.A pellentesque sit amet porttitor eget dolor morbi non arcu.Eleifend mi in nulla posuere sollicitudin.Nunc lobortis mattis aliquam faucibus.Adipiscing enim eu turpis egestas.Bibendum est ultricies integer quis auctor elit sed vulputate.In arcu cursus euismod quis viverra nibh cras pulvinar mattis.Proin nibh nisl condimentum id venenatis a condimentum vitae sapien.Phasellus egestas tellus rutrum tellus.Non arcu risus quis varius quam quisque id.Ac feugiat sed lectus vestibulum mattis.Nisl nisi scelerisque eu ultrices.Nulla at volutpat diam ut venenatis tellus.Curabitur gravida arcu ac tortor dignissim convallis aenean."

static int total_entries = 0;

static void test_write (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	assert (zip != NULL);

	assert (0 == zip_entry_open (zip, "test/test-1.txt"));
	assert (0 == zip_entry_write (zip, TESTDATA1, strlen (TESTDATA1)));
	assert (0 == strcmp (zip_entry_name (zip), "test/test-1.txt"));
	assert (total_entries == zip_entry_index (zip));
	++total_entries;
	assert (0 == zip_entry_close (zip));

	zip_close (zip);
}

static void test_append (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, ZIP_DEFAULT_COMPRESSION_LEVEL, 'a');
	assert (zip != NULL);

	assert (0 == zip_entry_open (zip, "test\\test-2.txt"));
	assert (0 == strcmp (zip_entry_name (zip), "test/test-2.txt"));
	assert (total_entries == zip_entry_index (zip));
	assert (0 == zip_entry_write (zip, TESTDATA2, strlen (TESTDATA2)));
	++total_entries;
	assert (0 == zip_entry_close (zip));

	assert (0 == zip_entry_open (zip, "test\\empty/"));
	assert (0 == strcmp (zip_entry_name (zip), "test/empty/"));
	assert (total_entries == zip_entry_index (zip));
	++total_entries;
	assert (0 == zip_entry_close (zip));

	assert (0 == zip_entry_open (zip, "empty/"));
	assert (0 == strcmp (zip_entry_name (zip), "empty/"));
	assert (total_entries == zip_entry_index (zip));
	++total_entries;
	assert (0 == zip_entry_close (zip));

	zip_close (zip);
}

static void test_read (void)
{
	char *buf = NULL;
	size_t bufsize;
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	assert (0 == zip_entry_open (zip, "test\\test-1.txt"));
	assert (0 == zip_entry_read (zip, (void **) &buf, &bufsize));
	assert (bufsize == strlen (TESTDATA1));
	assert (0 == strncmp (buf, TESTDATA1, bufsize));
	assert (0 == zip_entry_close (zip));
	free (buf);
	buf = NULL;
	bufsize = 0;

	assert (0 == zip_entry_open (zip, "test/test-2.txt"));
	assert (0 == zip_entry_read (zip, (void **) &buf, &bufsize));
	assert (bufsize == strlen (TESTDATA2));
	assert (0 == strncmp (buf, TESTDATA2, bufsize));
	assert (0 == zip_entry_close (zip));
	free (buf);
	buf = NULL;
	bufsize = 0;

	zip_close (zip);
}

struct buffer_t
{
	char *data;
	size_t size;
};

static size_t on_extract (void *arg, unsigned long long offset, const void *data,
						  size_t size)
{
	struct buffer_t *buf = ( struct buffer_t * )arg;
	buf->data = (char*)realloc (buf->data, buf->size + size + 1);
	assert (NULL != buf->data);

	memcpy (&( buf->data [buf->size] ), data, size);
	buf->size += size;
	buf->data [buf->size] = 0;

	return size;
}

static void test_extract (void)
{
	struct buffer_t buf = { 0 };

	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	assert (0 == zip_entry_open (zip, "test/test-1.txt"));
	assert (0 == zip_entry_extract (zip, on_extract, &buf));

	assert (buf.size == strlen (TESTDATA1));
	assert (0 == strncmp (buf.data, TESTDATA1, buf.size));
	assert (0 == zip_entry_close (zip));
	free (buf.data);
	buf.data = NULL;
	buf.size = 0;

	zip_close (zip);
}

static void test_total_entries (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	int n = zip_total_entries (zip);
	zip_close (zip);

	assert (n == total_entries);
}

static void test_entry_name (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	assert (zip_entry_name (zip) == NULL);

	assert (0 == zip_entry_open (zip, "test\\test-1.txt"));
	assert (NULL != zip_entry_name (zip));
	assert (0 == strcmp (zip_entry_name (zip), "test/test-1.txt"));
	assert (0 == zip_entry_close (zip));

	assert (0 == zip_entry_open (zip, "test/test-2.txt"));
	assert (NULL != zip_entry_name (zip));
	assert (0 == strcmp (zip_entry_name (zip), "test/test-2.txt"));
	assert (0 == zip_entry_close (zip));

	zip_close (zip);
}

static void test_entry_index (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	assert (0 == zip_entry_open (zip, "test\\test-1.txt"));
	assert (0 == zip_entry_index (zip));
	assert (0 == zip_entry_close (zip));

	assert (0 == zip_entry_open (zip, "test/test-2.txt"));
	assert (1 == zip_entry_index (zip));
	assert (0 == zip_entry_close (zip));

	zip_close (zip);
}

static void test_entry_openbyindex (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	assert (0 == zip_entry_openbyindex (zip, 1));
	assert (1 == zip_entry_index (zip));

	assert (0 == strcmp (zip_entry_name (zip), "test/test-2.txt"));
	assert (0 == zip_entry_close (zip));

	assert (0 == zip_entry_openbyindex (zip, 0));
	assert (0 == zip_entry_index (zip));
	assert (0 == strcmp (zip_entry_name (zip), "test/test-1.txt"));
	assert (0 == zip_entry_close (zip));

	zip_close (zip);
}

static void test_list_entries (void)
{
	struct zip_t *zip = zip_open (ZIPNAME, 0, 'r');
	assert (zip != NULL);

	int i = 0, n = zip_total_entries (zip);
	for ( ; i < n; ++i )
	{
		assert (0 == zip_entry_openbyindex (zip, i));
		fprintf (stdout, "[%d]: %s", i, zip_entry_name (zip));
		if ( zip_entry_isdir (zip) )
		{
			fprintf (stdout, " (DIR)");
		}
		fprintf (stdout, "\n");
		assert (0 == zip_entry_close (zip));
	}

	zip_close (zip);
}

int main (int argc, char *argv[])
{
	test_write ();
	test_append ();
	test_read ();
	test_extract ();
	test_total_entries ();
	test_entry_name ();
	test_entry_index ();
	test_entry_openbyindex ();
	test_list_entries ();

	return remove (ZIPNAME);
}

