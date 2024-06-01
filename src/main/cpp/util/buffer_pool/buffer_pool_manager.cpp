#include "buffer_pool_manager.h"

// TODO: use TableManager to refactor access to packed tables so that we have entire QueryTable metadata.  This will make it much easier to loop through the fields of a table for add/removeSequence and loadColumn.  BPM will no longer need to reference one pointer for every table/column pair.