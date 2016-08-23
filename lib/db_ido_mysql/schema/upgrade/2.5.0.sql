-- -----------------------------------------
-- upgrade path for Icinga 2.5.0
--
-- -----------------------------------------
-- Copyright (c) 2016 Icinga Development Team (http://www.icinga.org)
--
-- Please check http://docs.icinga.org for upgrading information!
-- -----------------------------------------

-- -----------------------------------------
-- #10069 IDO: check_source should not be a TEXT field
-- -----------------------------------------

ALTER TABLE icinga_hoststatus MODIFY COLUMN check_source varchar(255) character set latin1  default '';
ALTER TABLE icinga_servicestatus MODIFY COLUMN check_source varchar(255) character set latin1  default '';

-- -----------------------------------------
-- #10070
-- -----------------------------------------

CREATE INDEX idx_comments_object_id on icinga_comments(object_id);
CREATE INDEX idx_scheduleddowntime_object_id on icinga_scheduleddowntime(object_id);

-- -----------------------------------------
-- #11962
-- -----------------------------------------

ALTER TABLE icinga_hoststatus MODIFY COLUMN current_notification_number int unsigned default 0;
ALTER TABLE icinga_servicestatus MODIFY COLUMN current_notification_number int unsigned default 0;

-- -----------------------------------------
-- #10061
-- -----------------------------------------

ALTER TABLE icinga_contactgroups MODIFY COLUMN alias varchar(255) character set latin1  default '';
ALTER TABLE icinga_contacts MODIFY COLUMN alias varchar(255) character set latin1  default '';
ALTER TABLE icinga_hostgroups MODIFY COLUMN alias varchar(255) character set latin1  default '';
ALTER TABLE icinga_hosts MODIFY COLUMN alias varchar(255) character set latin1  default '';
ALTER TABLE icinga_servicegroups MODIFY COLUMN alias varchar(255) character set latin1  default '';
ALTER TABLE icinga_timeperiods MODIFY COLUMN alias varchar(255) character set latin1  default '';

-- -----------------------------------------
-- #10066
-- -----------------------------------------

CREATE INDEX idx_endpoints_object_id on icinga_endpoints(endpoint_object_id);
CREATE INDEX idx_endpointstatus_object_id on icinga_endpointstatus(endpoint_object_id);

CREATE INDEX idx_endpoints_zone_object_id on icinga_endpoints(zone_object_id);
CREATE INDEX idx_endpointstatus_zone_object_id on icinga_endpointstatus(zone_object_id);

CREATE INDEX idx_zones_object_id on icinga_zones(zone_object_id);
CREATE INDEX idx_zonestatus_object_id on icinga_zonestatus(zone_object_id);

CREATE INDEX idx_zones_parent_object_id on icinga_zones(parent_zone_object_id);
CREATE INDEX idx_zonestatus_parent_object_id on icinga_zonestatus(parent_zone_object_id);

-- -----------------------------------------
-- #12107
-- -----------------------------------------
CREATE INDEX idx_statehistory_cleanup on icinga_statehistory(instance_id, state_time);

-- -----------------------------------------
-- #12258
-- -----------------------------------------
ALTER TABLE icinga_comments ADD COLUMN session_token INTEGER default NULL;
ALTER TABLE icinga_scheduleddowntime ADD COLUMN session_token INTEGER default NULL;

CREATE INDEX idx_comments_session_del ON icinga_comments (instance_id, session_token);
CREATE INDEX idx_downtimes_session_del ON icinga_scheduleddowntime (instance_id, session_token);

-- -----------------------------------------
-- #12435
-- -----------------------------------------
ALTER TABLE icinga_commands ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_contactgroups ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_contacts ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_hostgroups ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_hosts ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_servicegroups ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_services ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_timeperiods ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_endpoints ADD config_hash VARCHAR(64) DEFAULT NULL;
ALTER TABLE icinga_zones ADD config_hash VARCHAR(64) DEFAULT NULL;

ALTER TABLE icinga_customvariables DROP session_token;
ALTER TABLE icinga_customvariablestatus DROP session_token;

CREATE INDEX idx_customvariables_object_id on icinga_customvariables(object_id);
CREATE INDEX idx_contactgroup_members_object_id on icinga_contactgroup_members(contact_object_id);
CREATE INDEX idx_hostgroup_members_object_id on icinga_hostgroup_members(host_object_id);
CREATE INDEX idx_servicegroup_members_object_id on icinga_servicegroup_members(service_object_id);
CREATE INDEX idx_servicedependencies_dependent_service_object_id on icinga_servicedependencies(dependent_service_object_id);
CREATE INDEX idx_hostdependencies_dependent_host_object_id on icinga_hostdependencies(dependent_host_object_id);
CREATE INDEX idx_service_contacts_service_id on icinga_service_contacts(service_id);
CREATE INDEX idx_host_contacts_host_id on icinga_host_contacts(host_id);

-- -----------------------------------------
-- set dbversion
-- -----------------------------------------
INSERT INTO icinga_dbversion (name, version, create_time, modify_time) VALUES ('idoutils', '1.14.1', NOW(), NOW()) ON DUPLICATE KEY UPDATE version='1.14.1', modify_time=NOW();
