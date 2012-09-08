CREATE TABLE "auth_identity" (
  "id" serial primary key,
  "version" integer not null,
  "auth_info_id" bigint,
  "provider" varchar(64) not null,
  "identity" varchar(512) not null
);
CREATE TABLE "auth_info" (
  "id" serial primary key,
  "version" integer not null,
  "user_id" bigint,
  "password_hash" varchar(100) not null,
  "password_method" varchar(20) not null,
  "password_salt" varchar(20) not null,
  "status" integer not null,
  "failed_login_attempts" integer not null,
  "last_login_attempt" timestamp,
  "email" varchar(256) not null,
  "unverified_email" varchar(256) not null,
  "email_token" varchar(64) not null,
  "email_token_expires" timestamp,
  "email_token_role" integer not null
);
CREATE TABLE "auth_token" (
  "id" serial primary key,
  "version" integer not null,
  "auth_info_id" bigint,
  "value" varchar(64) not null,
  "expires" timestamp
);
CREATE TABLE "thechess_bd" (
  "user_id" bigint,
  "type" integer not null,
  "value" varchar(240) not null,
  "used" timestamp,
  primary key ("user_id", "type", "value")
);
CREATE TABLE "thechess_comment" (
  "id" serial primary key,
  "version" integer not null,
  "show_index" double precision not null,
  "depth" integer not null,
  "type" integer not null,
  "state" integer not null,
  "text" text not null,
  "init_id" bigint,
  "root_id" bigint,
  "parent_id" bigint,
  "created" timestamp,
  "edited" timestamp,
  "ip" text not null,
  "score" integer not null
);
CREATE TABLE "thechess_competition" (
  "id" serial primary key,
  "version" integer not null,
  "name" text not null,
  "description" text not null,
  "init_id" bigint,
  "created" timestamp,
  "cp_id" bigint,
  "gp_id" bigint,
  "state" integer not null,
  "virtual_allower_id" bigint,
  "started" timestamp,
  "ended" timestamp,
  "comment_base_id" bigint
);
CREATE TABLE "thechess_cp" (
  "id" serial primary key,
  "version" integer not null,
  "name" text not null,
  "description" text not null,
  "init_id" bigint,
  "created" timestamp,
  "parent_id" bigint,
  "gp_id" bigint,
  "type" integer not null,
  "min_rating" integer not null,
  "max_rating" integer not null,
  "min_classification" integer not null,
  "max_classification" integer not null,
  "force_start_delay" interval,
  "min_users" integer not null,
  "max_users" integer not null,
  "min_recruiting_time" interval,
  "max_recruiting_time" interval,
  "max_simultaneous_games" integer not null,
  "games_factor" real not null,
  "relax_time" interval,
  "min_substages" integer not null,
  "increment_substages" integer not null,
  "min_online_time" interval,
  "max_online_time" interval
);
CREATE TABLE "thechess_game" (
  "id" serial primary key,
  "version" integer not null,
  "name" text not null,
  "description" text not null,
  "init_id" bigint,
  "created" timestamp,
  "gp_id" bigint,
  "game_moves" bytea not null,
  "state" integer not null,
  "white_id" bigint,
  "black_id" bigint,
  "winner_game_id" bigint,
  "competition_confirmer_white" boolean not null,
  "competition_confirmer_black" boolean not null,
  "colors_random" boolean not null,
  "confirmed" timestamp,
  "started" timestamp,
  "lastmove" timestamp,
  "ended" timestamp,
  "limit_private_white" interval,
  "limit_private_black" interval,
  "competition_id" bigint,
  "competition_stage" integer not null,
  "pause_until" timestamp,
  "pause_limit" interval,
  "pause_proposed_td" interval,
  "pause_proposer_id" bigint,
  "mistake_move" integer not null,
  "mistake_proposer_id" bigint,
  "draw_proposer_id" bigint,
  "rating_after_white" integer not null,
  "rating_after_black" integer not null,
  "comment_base_id" bigint,
  "norating" boolean not null
);
CREATE TABLE "thechess_gp" (
  "id" serial primary key,
  "version" integer not null,
  "name" text not null,
  "description" text not null,
  "init_id" bigint,
  "created" timestamp,
  "parent_id" bigint,
  "moves_moves" bytea not null,
  "limit_std" interval,
  "limit_private_init" interval,
  "norating" boolean not null,
  "pause_limit_init" interval,
  "first_draw" integer not null
);
CREATE TABLE "thechess_ip_ban" (
  "id" serial primary key,
  "version" integer not null,
  "ip" text not null,
  "enabled" boolean not null,
  "start" timestamp,
  "stop" timestamp,
  "reason" text not null,
  "creator_id" bigint
);
CREATE TABLE "thechess_user" (
  "id" serial primary key,
  "version" integer not null,
  "username" text not null,
  "rights" integer not null,
  "sessions" integer not null,
  "last_enter" timestamp,
  "online_time" interval,
  "classification" integer not null,
  "classification_confirmer_id" bigint,
  "games_stat_elo" integer not null,
  "games_stat_all" integer not null,
  "games_stat_wins" integer not null,
  "games_stat_fails" integer not null,
  "compteitions_stat_elo" integer not null,
  "compteitions_stat_all" integer not null,
  "compteitions_stat_wins" integer not null,
  "compteitions_stat_fails" integer not null,
  "comment_base_id" bigint,
  "settings" integer not null,
  "karma" integer not null,
  "registration_date" timestamp,
  "avatar_path" text not null,
  "filter_min_online" interval,
  "description" text not null,
  "locale" varchar(5) not null,
  "vacation_until" timestamp
);
CREATE TABLE "winners_competition" (
  "thechess_user_id" bigint not null,
  "thechess_competition_id" bigint not null,
  primary key ("thechess_user_id", "thechess_competition_id"),
  constraint "fk_winners_competition_key1" foreign key ("thechess_user_id") references "thechess_user" ("id"),
  constraint "fk_winners_competition_key2" foreign key ("thechess_competition_id") references "thechess_competition" ("id")
);
CREATE TABLE "members_competitions" (
  "thechess_user_id" bigint not null,
  "thechess_competition_id" bigint not null,
  primary key ("thechess_user_id", "thechess_competition_id"),
  constraint "fk_members_competitions_key1" foreign key ("thechess_user_id") references "thechess_user" ("id"),
  constraint "fk_members_competitions_key2" foreign key ("thechess_competition_id") references "thechess_competition" ("id")
);
CREATE TABLE "thechess_message_filter" (
  "good_id" bigint not null,
  "bad_id" bigint not null,
  primary key ("good_id", "bad_id"),
  constraint "fk_thechess_message_filter_key1" foreign key ("good_id") references "thechess_user" ("id"),
  constraint "fk_thechess_message_filter_key2" foreign key ("bad_id") references "thechess_user" ("id")
);

ALTER TABLE "auth_identity"
  ADD constraint "fk_auth_identity_auth_info" foreign key ("auth_info_id") references "auth_info" ("id") on delete cascade;
ALTER TABLE "auth_info"
  ADD constraint "fk_auth_info_user" foreign key ("user_id") references "thechess_user" ("id") on delete cascade;
ALTER TABLE "auth_token"
  ADD constraint "fk_auth_token_auth_info" foreign key ("auth_info_id") references "auth_info" ("id") on delete cascade;
ALTER TABLE "thechess_bd"
  ADD constraint "fk_thechess_bd_user" foreign key ("user_id") references "thechess_user" ("id");
ALTER TABLE "thechess_comment"
  ADD constraint "fk_thechess_comment_init" foreign key ("init_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_comment_root" foreign key ("root_id") references "thechess_comment" ("id"),
  ADD constraint "fk_thechess_comment_parent" foreign key ("parent_id") references "thechess_comment" ("id");
ALTER TABLE "thechess_competition"
  ADD constraint "fk_thechess_competition_init" foreign key ("init_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_competition_cp" foreign key ("cp_id") references "thechess_cp" ("id"),
  ADD constraint "fk_thechess_competition_gp" foreign key ("gp_id") references "thechess_gp" ("id"),
  ADD constraint "fk_thechess_competition_virtual_allower" foreign key ("virtual_allower_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_competition_comment_base" foreign key ("comment_base_id") references "thechess_comment" ("id");
ALTER TABLE "thechess_cp"
  ADD constraint "fk_thechess_cp_init" foreign key ("init_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_cp_parent" foreign key ("parent_id") references "thechess_cp" ("id"),
  ADD constraint "fk_thechess_cp_gp" foreign key ("gp_id") references "thechess_gp" ("id");
ALTER TABLE "thechess_game"
  ADD constraint "fk_thechess_game_init" foreign key ("init_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_gp" foreign key ("gp_id") references "thechess_gp" ("id"),
  ADD constraint "fk_thechess_game_white" foreign key ("white_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_black" foreign key ("black_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_winner_game" foreign key ("winner_game_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_competition" foreign key ("competition_id") references "thechess_competition" ("id"),
  ADD constraint "fk_thechess_game_pause_proposer" foreign key ("pause_proposer_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_mistake_proposer" foreign key ("mistake_proposer_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_draw_proposer" foreign key ("draw_proposer_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_game_comment_base" foreign key ("comment_base_id") references "thechess_comment" ("id");
ALTER TABLE "thechess_gp"
  ADD constraint "fk_thechess_gp_init" foreign key ("init_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_gp_parent" foreign key ("parent_id") references "thechess_gp" ("id");
ALTER TABLE "thechess_ip_ban"
  ADD constraint "fk_thechess_ip_ban_creator" foreign key ("creator_id") references "thechess_user" ("id");
ALTER TABLE "thechess_user"
  ADD constraint "fk_thechess_user_classification_confirmer" foreign key ("classification_confirmer_id") references "thechess_user" ("id"),
  ADD constraint "fk_thechess_user_comment_base" foreign key ("comment_base_id") references "thechess_comment" ("id");


CREATE INDEX "members_competitions_thechess_competition" on "members_competitions" ("thechess_competition_id");
CREATE INDEX "members_competitions_thechess_user" on "members_competitions" ("thechess_user_id");
CREATE INDEX "thechess_message_filter_thechess_user_bad_id" on "thechess_message_filter" ("bad_id");
CREATE INDEX "thechess_message_filter_thechess_user_good_id" on "thechess_message_filter" ("good_id");
CREATE INDEX "winners_competition_thechess_competition" on "winners_competition" ("thechess_competition_id");
CREATE INDEX "winners_competition_thechess_user" on "winners_competition" ("thechess_user_id");

