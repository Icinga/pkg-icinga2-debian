-- -----------------------------------------
-- upgrade path for Icinga 2.2.0
--
-- -----------------------------------------
-- Copyright (c) 2014 Icinga Development Team (http://www.icinga.org)
--
-- Please check http://docs.icinga.org for upgrading information!
-- -----------------------------------------

ALTER TABLE icinga_programstatus ADD COLUMN program_version TEXT default NULL;

ALTER TABLE icinga_customvariables ADD COLUMN is_json INTEGER default 0;
ALTER TABLE icinga_customvariablestatus ADD COLUMN is_json INTEGER default 0;


-- -----------------------------------------
-- update dbversion
-- -----------------------------------------

SELECT updatedbversion('1.12.0');

