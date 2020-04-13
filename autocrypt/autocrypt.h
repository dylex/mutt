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

#ifndef _AUTOCRYPT_H
#define _AUTOCRYPT_H 1

#include <sqlite3.h>

WHERE sqlite3 *AutocryptDB;

typedef struct
{
  char *email_addr;
  char *keyid;
  char *keydata;
  int prefer_encrypt;    /* 0 = nopref, 1 = mutual */
  int enabled;
} AUTOCRYPT_ACCOUNT;

typedef struct
{
  char *email_addr;
  sqlite3_int64 last_seen;
  sqlite3_int64 autocrypt_timestamp;
  char *keyid;
  char *keydata;
  int prefer_encrypt;    /* 0 = nopref, 1 = mutual */
  sqlite3_int64 gossip_timestamp;
  char *gossip_keyid;
  char *gossip_keydata;
} AUTOCRYPT_PEER;

typedef struct
{
  char *peer_email_addr;
  char *email_msgid;
  sqlite3_int64 timestamp;
  char *keydata;
} AUTOCRYPT_PEER_HISTORY;

typedef struct
{
  char *peer_email_addr;
  char *sender_email_addr;
  char *email_msgid;
  sqlite3_int64 timestamp;
  char *gossip_keydata;
} AUTOCRYPT_GOSSIP_HISTORY;

typedef enum
{
  AUTOCRYPT_REC_OFF,
  AUTOCRYPT_REC_NO,
  AUTOCRYPT_REC_DISCOURAGE,
  AUTOCRYPT_REC_AVAILABLE,
  AUTOCRYPT_REC_YES
} autocrypt_rec_t;

int mutt_autocrypt_init (int);
void mutt_autocrypt_cleanup (void);
int mutt_autocrypt_process_autocrypt_header (HEADER *hdr, ENVELOPE *env);
int mutt_autocrypt_process_gossip_header (HEADER *hdr, ENVELOPE *prot_headers);
autocrypt_rec_t mutt_autocrypt_ui_recommendation (HEADER *hdr, char **keylist);
int mutt_autocrypt_set_sign_as_default_key (HEADER *hdr);
int mutt_autocrypt_write_autocrypt_header (ENVELOPE *env, FILE *fp);
int mutt_autocrypt_write_gossip_headers (ENVELOPE *env, FILE *fp);
int mutt_autocrypt_generate_gossip_list (HEADER *hdr);
void mutt_autocrypt_account_menu (void);

#endif
