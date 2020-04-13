/*
 * Copyright (C) 2019 Kevin J. McCarthy <kevin@8t8.us>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "mutt.h"
#include "autocrypt.h"
#include "autocrypt_private.h"

int mutt_autocrypt_schema_init (void)
{
  const char *schema;
  char *errmsg = NULL;

  schema =
    "BEGIN TRANSACTION; "

    "CREATE TABLE account ("
    "email_addr text primary key not null, "
    "keyid text, "
    "keydata text, "
    "prefer_encrypt int, "
    "enabled int);"

    "CREATE TABLE peer ("
    "email_addr text primary key not null, "
    "last_seen int, "
    "autocrypt_timestamp int, "
    "keyid text, "
    "keydata text, "
    "prefer_encrypt int, "
    "gossip_timestamp int, "
    "gossip_keyid text, "
    "gossip_keydata text);"

    "CREATE TABLE peer_history ("
    "peer_email_addr text not null, "
    "email_msgid text, "
    "timestamp int, "
    "keydata text);"

    "CREATE INDEX peer_history_email "
    "ON peer_history ("
    "peer_email_addr);"

    "CREATE TABLE gossip_history ("
    "peer_email_addr text not null, "
    "sender_email_addr text, "
    "email_msgid text, "
    "timestamp int, "
    "gossip_keydata text);"

    "CREATE INDEX gossip_history_email "
    "ON gossip_history ("
    "peer_email_addr);"

    "CREATE TABLE schema ("
    "version int);"

    "INSERT into schema (version) values (1);"

    "COMMIT TRANSACTION";

  if (sqlite3_exec (AutocryptDB, schema, NULL, NULL, &errmsg) != SQLITE_OK)
  {
    dprint (1, (debugfile, "mutt_autocrypt_schema_init() returned %s\n", errmsg));
    sqlite3_free (errmsg);
    return -1;
  }
  return 0;
}

int mutt_autocrypt_schema_update (void)
{
  sqlite3_stmt *stmt = NULL;
  int rv = -1, version;

  if (sqlite3_prepare_v2 (
        AutocryptDB,
        "SELECT version FROM schema;",
        -1,
        &stmt,
        NULL) != SQLITE_OK)
    goto cleanup;

  if (sqlite3_step (stmt) != SQLITE_ROW)
    goto cleanup;

  version = sqlite3_column_int (stmt, 0);

  if (version > 1)
  {
    /* L10N:
       The autocrypt database keeps track of schema version numbers.
       This error occurs if the version number is too high.
       Presumably because this is an old version of mutt and the
       database was upgraded by a future version.
    */
    mutt_error _("Autocrypt database version is too new");
    mutt_sleep (0);
    goto cleanup;
  }

  /* TODO: schema version upgrades go here.
   * Bump one by one, each update inside a transaction. */

  rv = 0;

cleanup:
  sqlite3_finalize (stmt);
  return rv;
}
