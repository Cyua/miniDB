# WickyDB #
a mini SQL database for our course.
## CatalogManager ##
This class is used to manage the catalog information like `table`, `column` and `type`. And here is its member function.

## Index Block Layout
For every block in the disk, the first block maintain the whole storage information 
including the block usage and the hole in the file.
the first the integer is the last block we used