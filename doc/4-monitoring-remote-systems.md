# <a id="monitoring-remote-systems"></a> Monitoring Remote Systems

## <a id="agent-less-checks"></a> Agent-less Checks

If the remote service is available using a network protocol and port,
and a [check plugin](#setting-up-check-plugins) is available, you don't
necessarily need a local client installed. Rather choose a plugin and
configure all parameters and thresholds. The [Icinga 2 Template Library](#itl)
already ships various examples.

## <a id="agent-based-checks"></a> Agent-based Checks

If the remote services are not directly accessible through the network, a
local agent installation exposing the results to check queries can
become handy.

### <a id="agent-based-checks-snmp"></a> SNMP

The SNMP daemon runs on the remote system and answers SNMP queries by plugin
binaries. The [Monitoring Plugins package](#setting-up-check-plugins) ships
the `check_snmp` plugin binary, but there are plenty of [existing plugins](#integrate-additional-plugins)
for specific use cases already around, for example monitoring Cisco routers.

The following example uses the [SNMP ITL](#itl-snmp) `CheckCommand` and just
overrides the `snmp_oid` custom attribute. A service is created for all hosts which
have the `snmp-community` custom attribute.

    apply Service "uptime" {
      import "generic-service"

      check_command = "snmp"
      vars.snmp_oid = "1.3.6.1.2.1.1.3.0"

      assign where host.vars.snmp_community != ""
    }

### <a id="agent-based-checks-ssh"></a> SSH

Calling a plugin using the SSH protocol to execute a plugin on the remote server fetching
its return code and output. The `by_ssh` command object is part of the built-in templates and
requires the `check_by_ssh` check plugin which is available in the [Monitoring Plugins package](#setting-up-check-plugins).

    object CheckCommand "by_ssh_swap" {
      import "by_ssh"

      vars.by_ssh_command = "/usr/lib/nagios/plugins/check_swap -w $by_ssh_swap_warn$ -c $by_ssh_swap_crit$"
      vars.by_ssh_swap_warn = "75%"
      vars.by_ssh_swap_crit = "50%"
    }

    object Service "swap" {
      import "generic-service"

      host_name = "remote-ssh-host"

      check_command = "by_ssh_swap"

      vars.by_ssh_logname = "icinga"
    }

### <a id="agent-based-checks-nrpe"></a> NRPE

[NRPE](http://docs.icinga.org/latest/en/nrpe.html) runs as daemon on the remote client including
the required plugins and command definitions.
Icinga 2 calls the `check_nrpe` plugin binary in order to query the configured command on the
remote client.

The NRPE daemon uses its own configuration format in nrpe.cfg while `check_nrpe`
can be embedded into the Icinga 2 `CheckCommand` configuration syntax.

You can use the `check_nrpe` plugin from the NRPE project to query the NRPE daemon.
Icinga 2 provides the [nrpe check command](#plugin-check-command-nrpe) for this:

Example:

    object Service "users" {
      import "generic-service"

      host_name = "remote-nrpe-host"

      check_command = "nrpe"
      vars.nrpe_command = "check_users"
    }

nrpe.cfg:

    command[check_users]=/usr/local/icinga/libexec/check_users -w 5 -c 10

### <a id="agent-based-checks-nsclient"></a> NSClient++

[NSClient++](http://nsclient.org) works on both Windows and Linux platforms and is well
known for its magnificent Windows support. There are alternatives like the WMI interface,
but using `NSClient++` will allow you to run local scripts similar to check plugins fetching
the required output and performance counters.

You can use the `check_nt` plugin from the Monitoring Plugins project to query NSClient++.
Icinga 2 provides the [nscp check command](#plugin-check-command-nscp) for this:

Example:

    object Service "disk" {
      import "generic-service"

      host_name = "remote-windows-host"

      check_command = "nscp"

      vars.nscp_variable = "USEDDISKSPACE"
      vars.nscp_params = "c"
      vars.nscp_warn = 70
      vars.nscp_crit = 80
    }

For details on the `NSClient++` configuration please refer to the [official documentation](http://www.nsclient.org/nscp/wiki/doc/configuration/0.4.x).

### <a id="agent-based-checks-icinga2-agent"></a> Icinga 2 Agent

A dedicated Icinga 2 agent supporting all platforms and using the native
Icinga 2 communication protocol supported with SSL certificates, IPv4/IPv6
support, etc. is on the [development roadmap](https://dev.icinga.org/projects/i2?jump=issues).
Meanwhile remote checkers in a [cluster](#distributed-monitoring-high-availability) setup could act as
immediate replacement, but without any local configuration - or pushing
their standalone configuration back to the master node including their check
result messages.

> **Note**
>
> Remote checker instances are independent Icinga 2 instances which schedule
> their checks and just synchronize them back to the defined master zone.

### <a id="agent-based-checks-snmp-traps"></a> Passive Check Results and SNMP Traps

SNMP Traps can be received and filtered by using [SNMPTT](http://snmptt.sourceforge.net/) and specific trap handlers
passing the check results to Icinga 2.

> **Note**
>
> The host and service object configuration must be available on the Icinga 2
> server in order to process passive check results.

### <a id="agent-based-checks-nsca-ng"></a> NSCA-NG

[NSCA-ng](http://www.nsca-ng.org) provides a client-server pair that allows the
remote sender to push check results into the Icinga 2 `ExternalCommandListener`
feature.

> **Note**
>
> This addon works in a similar fashion like the Icinga 1.x distributed model. If you
> are looking for a real distributed architecture with Icinga 2, scroll down.


## <a id="distributed-monitoring-high-availability"></a> Distributed Monitoring and High Availability

Building distributed environments with high availability included is fairly easy with Icinga 2.
The cluster feature is built-in and allows you to build many scenarios based on your requirements:

* [High Availability](#cluster-scenarios-high-availability). All instances in the `Zone` elect one active master and run as Active/Active cluster.
* [Distributed Zones](#cluster-scenarios-distributed-zones). A master zone and one or more satellites in their zones.
* [Load Distribution](#cluster-scenarios-load-distribution). A configuration master and multiple checker satellites.

You can combine these scenarios into a global setup fitting your requirements.

Each instance got their own event scheduler, and does not depend on a centralized master
coordinating and distributing the events. In case of a cluster failure, all nodes
continue to run independently. Be alarmed when your cluster fails and a Split-Brain-scenario
is in effect - all alive instances continue to do their job, and history will begin to differ.

> ** Note **
>
> Before you start, make sure to read the [requirements](#distributed-monitoring-requirements).


### <a id="cluster-requirements"></a> Cluster Requirements

Before you start deploying, keep the following things in mind:

* Your [SSL CA and certificates](#certificate-authority-certificates) are mandatory for secure communication
* Get pen and paper or a drawing board and design your nodes and zones!
    * all nodes in a cluster zone are providing high availability functionality and trust each other
    * cluster zones can be built in a Top-Down-design where the child trusts the parent
    * communication between zones happens bi-directional which means that a DMZ-located node can still reach the master node, or vice versa
* Update firewall rules and ACLs
* Decide whether to use the built-in [configuration syncronization](#cluster-zone-config-sync) or use an external tool (Puppet, Ansible, Chef, Salt, etc) to manage the configuration deployment


> **Tip**
>
> If you're looking for troubleshooting cluster problems, check the general
> [troubleshooting](#troubleshooting-cluster) section.

#### <a id="cluster-naming-convention"></a> Cluster Naming Convention

The SSL certificate common name (CN) will be used by the [ApiListener](#objecttype-apilistener)
object to determine the local authority. This name must match the local [Endpoint](#objecttype-endpoint)
object name.

Example:

    # icinga2-build-key icinga2a
    ...
    Common Name (e.g. server FQDN or YOUR name) [icinga2a]:

    # vim cluster.conf

    object Endpoint "icinga2a" {
      host = "icinga2a.icinga.org"
    }

The [Endpoint](#objecttype-endpoint) name is further referenced as `endpoints` attribute on the
[Zone](objecttype-zone) object.

    object Endpoint "icinga2b" {
      host = "icinga2b.icinga.org"
    }

    object Zone "config-ha-master" {
      endpoints = [ "icinga2a", "icinga2b" ]
    }

Specifying the local node name using the [NodeName](#configure-nodename) variable requires
the same name as used for the endpoint name and common name above. If not set, the FQDN is used.

    const NodeName = "icinga2a"


### <a id="certificate-authority-certificates"></a> Certificate Authority and Certificates

Icinga 2 ships two scripts assisting with CA and node certificate creation
for your Icinga 2 cluster.

> **Note**
>
> You're free to use your own method to generated a valid ca and signed client
> certificates.

Please make sure to export the environment variable `ICINGA_CA` pointing to
an empty folder for the newly created CA files:

    # export ICINGA_CA="/root/icinga-ca"

The scripts will put all generated data and the required certificates in there.

The first step is the creation of the certificate authority (CA) running the
following command:

    # icinga2-build-ca

Now create a certificate and key file for each node running the following command
(replace `icinga2a` with the required hostname):

    # icinga2-build-key icinga2a

Repeat the step for all nodes in your cluster scenario.

Save the CA key in a secure location in case you want to set up certificates for
additional nodes at a later time.

Navigate to the location of your newly generated certificate files, and manually
copy/transfer them to `/etc/icinga2/pki` in your Icinga 2 configuration folder.

> **Note**
>
> The certificate files must be readable by the user Icinga 2 is running as. Also,
> the private key file must not be world-readable.

Each node requires the following files in `/etc/icinga2/pki` (replace `fqdn-nodename` with
the host's FQDN):

* ca.crt
* &lt;fqdn-nodename&gt;.crt
* &lt;fqdn-nodename&gt;.key


### <a id="cluster-configuration"></a> Cluster Configuration

The following section describe which configuration must be updated/created
in order to get your cluster running with basic functionality.

* [configure the node name](#configure-nodename)
* [configure the ApiListener object](#configure-apilistener-object)
* [configure cluster endpoints](#configure-cluster-endpoints)
* [configure cluster zones](#configure-cluster-zones)

Once you're finished with the basic setup the following section will
describe how to use [zone configuration synchronisation](#cluster-zone-config-sync)
and configure [cluster scenarios](#cluster-scenarios).

#### <a id="configure-nodename"></a> Configure the Icinga Node Name

Instead of using the default FQDN as node name you can optionally set
that value using the [NodeName](#global-constants) constant.

> ** Note **
>
> Skip this step if your FQDN already matches the default `NodeName` set
> in `/etc/icinga2/constants.conf`.

This setting must be unique for each node, and must also match
the name of the local [Endpoint](#objecttype-endpoint) object and the
SSL certificate common name as described in the
[cluster naming convention](#cluster-naming-convention).

    vim /etc/icinga2/constants.conf

    /* Our local instance name. By default this is the server's hostname as returned by `hostname --fqdn`.
     * This should be the common name from the API certificate.
     */
    const NodeName = "icinga2a"


Read further about additional [naming conventions](#cluster-naming-convention).

Not specifying the node name will make Icinga 2 using the FQDN. Make sure that all
configured endpoint names and common names are in sync.

#### <a id="configure-apilistener-object"></a> Configure the ApiListener Object

The [ApiListener](#objecttype-apilistener) object needs to be configured on
every node in the cluster with the following settings:

A sample config looks like:

    object ApiListener "api" {
      cert_path = SysconfDir + "/icinga2/pki/" + NodeName + ".crt"
      key_path = SysconfDir + "/icinga2/pki/" + NodeName + ".key"
      ca_path = SysconfDir + "/icinga2/pki/ca.crt"
      accept_config = true
    }

You can simply enable the `api` feature using

    # icinga2-enable-feature api

Edit `/etc/icinga2/features-enabled/api.conf` if you require the configuration
synchronisation enabled for this node. Set the `accept_config` attribute to `true`.

> **Note**
>
> The certificate files must be readable by the user Icinga 2 is running as. Also,
> the private key file must not be world-readable.

#### <a id="configure-cluster-endpoints"></a> Configure Cluster Endpoints

`Endpoint` objects specify the `host` and `port` settings for the cluster nodes.
This configuration can be the same on all nodes in the cluster only containing
connection information.

A sample configuration looks like:

    /**
     * Configure config master endpoint
     */

    object Endpoint "icinga2a" {
      host = "icinga2a.icinga.org"
    }

If this endpoint object is reachable on a different port, you must configure the
`ApiListener` on the local `Endpoint` object accordingly too.

#### <a id="configure-cluster-zones"></a> Configure Cluster Zones

`Zone` objects specify the endpoints located in a zone. That way your distributed setup can be
seen as zones connected together instead of multiple instances in that specific zone.

Zones can be used for [high availability](#cluster-scenarios-high-availability),
[distributed setups](#cluster-scenarios-distributed-zones) and
[load distribution](#cluster-scenarios-load-distribution).

Each Icinga 2 `Endpoint` must be put into its respective `Zone`. In this example, you will
define the zone `config-ha-master` where the `icinga2a` and `icinga2b` endpoints
are located. The `check-satellite` zone consists of `icinga2c` only, but more nodes could
be added.

The `config-ha-master` zone acts as High-Availability setup - the Icinga 2 instances elect
one active master where all features are running on (for example `icinga2a`). In case of
failure of the `icinga2a` instance, `icinga2b` will take over automatically.

    object Zone "config-ha-master" {
      endpoints = [ "icinga2a", "icinga2b" ]
    }

The `check-satellite` zone is a separated location and only sends back their checkresults to
the defined parent zone `config-ha-master`.

    object Zone "check-satellite" {
      endpoints = [ "icinga2c" ]
      parent = "config-ha-master"
    }


### <a id="cluster-zone-config-sync"></a> Zone Configuration Synchronisation

By default all objects for specific zones should be organized in

    /etc/icinga2/zones.d/<zonename>

on the configuration master.

Your child zones and endpoint members **must not** have their config copied to `zones.d`.
The built-in configuration synchronisation takes care of that if your nodes accept
configuration from the parent zone. You can define that in the
[ApiListener](#configure-apilistener-object) object by configuring the `accept_config`
attribute accordingly.

You should remove the sample config included in `conf.d` by commenting the `recursive_include`
statement in [icinga2.conf](#icinga2-conf):

    //include_recursive "conf.d"

Better use a dedicated directory name like `cluster` or similar, and include that
one if your nodes require local configuration not being synced to other nodes. That's
useful for local [health checks](#cluster-health-check) for example.

> **Note**
>
> In a [high availability](#cluster-scenarios-high-availability)
> setup only one assigned node can act as configuration master. All other zone
> member nodes **must not** have the `/etc/icinga2/zones.d` directory populated.

These zone packages are then distributed to all nodes in the same zone, and
to their respective target zone instances.

Each configured zone must exist with the same directory name. The parent zone
syncs the configuration to the child zones, if allowed using the `accept_config`
attribute of the [ApiListener](#configure-apilistener-object) object.

Config on node `icinga2a`:

    object Zone "master" {
      endpoints = [ "icinga2a" ]
    }

    object Zone "checker" {
      endpoints = [ "icinga2b" ]
      parent = "master"
    }

    /etc/icinga2/zones.d
      master
        health.conf
      checker
        health.conf
        demo.conf

Config on node `icinga2b`:

    object Zone "master" {
      endpoints = [ "icinga2a" ]
    }

    object Zone "checker" {
      endpoints = [ "icinga2b" ]
      parent = "master"
    }

    /etc/icinga2/zones.d
      EMPTY_IF_CONFIG_SYNC_ENABLED

If the local configuration is newer than the received update Icinga 2 will skip the synchronisation
process.

> **Note**
>
> `zones.d` must not be included in [icinga2.conf](#icinga2-conf). Icinga 2 automatically
> determines the required include directory. This can be overridden using the
> [global constant](#global-constants) `ZonesDir`.

#### <a id="zone-global-config-templates"></a> Global Configuration Zone for Templates

If your zone configuration setup shares the same templates, groups, commands, timeperiods, etc.
you would have to duplicate quite a lot of configuration objects making the merged configuration
on your configuration master unique.

> ** Note **
>
> Only put templates, groups, etc into this zone. DO NOT add checkable objects such as
> hosts or services here. If they are checked by all instances globally, this will lead
> into duplicated check results and unclear state history. Not easy to troubleshoot too -
> you've been warned.

That is not necessary by defining a global zone shipping all those templates. By setting
`global = true` you ensure that this zone serving common configuration templates will be
synchronized to all involved nodes (only if they accept configuration though).

Config on configuration master:

    /etc/icinga2/zones.d
      global-templates/
        templates.conf
        groups.conf
      master
        health.conf
      checker
        health.conf
        demo.conf

In this example, the global zone is called `global-templates` and must be defined in
your zone configuration visible to all nodes.

    object Zone "global-templates" {
      global = true
    }

> **Note**
>
> If the remote node does not have this zone configured, it will ignore the configuration
> update, if it accepts synchronized configuration.

If you don't require any global configuration, skip this setting.

#### <a id="zone-config-sync-permissions"></a> Zone Configuration Synchronisation Permissions

Each [ApiListener](#objecttype-apilistener) object must have the `accept_config` attribute
set to `true` to receive configuration from the parent `Zone` members. Default value is `false`.

    object ApiListener "api" {
      cert_path = SysconfDir + "/icinga2/pki/" + NodeName + ".crt"
      key_path = SysconfDir + "/icinga2/pki/" + NodeName + ".key"
      ca_path = SysconfDir + "/icinga2/pki/ca.crt"
      accept_config = true
    }

If `accept_config` is set to `false`, this instance won't accept configuration from remote
master instances anymore.

> ** Tip **
>
> Look into the [troubleshooting guides](#troubleshooting-cluster-config-sync) for debugging
> problems with the configuration synchronisation.


### <a id="cluster-health-check"></a> Cluster Health Check

The Icinga 2 [ITL](#itl) ships an internal check command checking all configured
`EndPoints` in the cluster setup. The check result will become critical if
one or more configured nodes are not connected.

Example:

    object Service "cluster" {
        check_command = "cluster"
        check_interval = 5s
        retry_interval = 1s

        host_name = "icinga2a"
    }

Each cluster node should execute its own local cluster health check to
get an idea about network related connection problems from different
points of view.

Additionally you can monitor the connection from the local zone to the remote
connected zones.

Example for the `checker` zone checking the connection to the `master` zone:

    object Service "cluster-zone-master" {
      check_command = "cluster-zone"
      check_interval = 5s
      retry_interval = 1s
      vars.cluster_zone = "master"

      host_name = "icinga2b"
    }


### <a id="cluster-scenarios"></a> Cluster Scenarios

All cluster nodes are full-featured Icinga 2 instances. You only need to enabled
the features for their role (for example, a `Checker` node only requires the `checker`
feature enabled, but not `notification` or `ido-mysql` features).

#### <a id="cluster-scenarios-security"></a> Security in Cluster Scenarios

While there are certain capabilities to ensure the safe communication between all
nodes (firewalls, policies, software hardening, etc) the Icinga 2 cluster also provides
additional security itself:

* [SSL certificates](#certificate-authority-certificates) are mandatory for cluster communication.
* Child zones only receive event updates (check results, commands, etc) for their configured updates.
* Zones cannot influence/interfere other zones. Each checked object is assigned to only one zone.
* All nodes in a zone trust each other.
* [Configuration sync](#zone-config-sync-permissions) is disabled by default.

#### <a id="cluster-scenarios-features"></a> Features in Cluster Zones

Each cluster zone may use all available features. If you have multiple locations
or departments, they may write to their local database, or populate graphite.
Even further all commands are distributed amongst connected nodes. For example, you could
re-schedule a check or acknowledge a problem on the master, and it gets replicated to the
actual slave checker node.

DB IDO on the left, graphite on the right side - works (if you disable
[DB IDO HA](#high-availability-db-ido)).
Icinga Web 2 on the left, checker and notifications on the right side - works too.
Everything on the left and on the right side - make sure to deal with
[load-balanced notifications and checks](#high-availability-features) in a
[HA zone](#cluster-scenarios-high-availability).
configure-cluster-zones
#### <a id="cluster-scenarios-distributed-zones"></a> Distributed Zones

That scenario fits if your instances are spread over the globe and they all report
to a master instance. Their network connection only works towards the master master
(or the master is able to connect, depending on firewall policies) which means
remote instances won't see each/connect to each other.

All events (check results, downtimes, comments, etc) are synced to the master node,
but the remote nodes can still run local features such as a web interface, reporting,
graphing, etc. in their own specified zone.

Imagine the following example with a master node in Nuremberg, and two remote DMZ
based instances in Berlin and Vienna. Additonally you'll specify
[global templates](#zone-global-config-templates) available in all zones.

The configuration tree on the master instance `nuremberg` could look like this:

    zones.d
      global-templates/
        templates.conf
        groups.conf
      nuremberg/
        local.conf
      berlin/
        hosts.conf
      vienna/
        hosts.conf

The configuration deployment will take care of automatically synchronising
the child zone configuration:

* The master node sends `zones.d/berlin` to the `berlin` child zone.
* The master node sends `zones.d/vienna` to the `vienna` child zone.
* The master node sends `zones.d/global-templates` to the `vienna` and `berlin` child zones.

The endpoint configuration would look like:

    object Endpoint "nuremberg-master" {
      host = "nuremberg.icinga.org"
    }

    object Endpoint "berlin-satellite" {
      host = "berlin.icinga.org"
    }

    object Endpoint "vienna-satellite" {
      host = "vienna.icinga.org"
    }

The zones would look like:

    object Zone "nuremberg" {
      endpoints = [ "nuremberg-master" ]
    }

    object Zone "berlin" {
      endpoints = [ "berlin-satellite" ]
      parent = "nuremberg"
    }

    object Zone "vienna" {
      endpoints = [ "vienna-satellite" ]
      parent = "nuremberg"
    }

    object Zone "global-templates" {
      global = true
    }

The `nuremberg-master` zone will only execute local checks, and receive
check results from the satellite nodes in the zones `berlin` and `vienna`.

> **Note**
>
> The child zones `berlin` and `vienna` will get their configuration synchronised
> from the configuration master 'nuremberg'. The endpoints in the child
> zones **must not** have their `zones.d` directory populated if this endpoint
> [accepts synced configuration](#zone-config-sync-permissions).

#### <a id="cluster-scenarios-load-distribution"></a> Load Distribution

If you are planning to off-load the checks to a defined set of remote workers
you can achieve that by:

* Deploying the configuration on all nodes.
* Let Icinga 2 distribute the load amongst all available nodes.

That way all remote check instances will receive the same configuration
but only execute their part. The master instance located in the `master` zone
can also execute checks, but you may also disable the `Checker` feature.

Configuration on the master node:

    zones.d/
      global-templates/
      master/
      checker/

If you are planning to have some checks executed by a specific set of checker nodes
you have to define additional zones and define these check objects there.

Endpoints:

    object Endpoint "master-node" {
      host = "master.icinga.org"
    }

    object Endpoint "checker1-node" {
      host = "checker1.icinga.org"
    }

    object Endpoint "checker2-node" {
      host = "checker2.icinga.org"
    }


Zones:

    object Zone "master" {
      endpoints = [ "master-node" ]
    }

    object Zone "checker" {
      endpoints = [ "checker1-node", "checker2-node" ]
      parent = "master"
    }

    object Zone "global-templates" {
      global = true
    }

> **Note**
>
> The child zones `checker` will get its configuration synchronised
> from the configuration master 'master'. The endpoints in the child
> zone **must not** have their `zones.d` directory populated if this endpoint
> [accepts synced configuration](#zone-config-sync-permissions).

#### <a id="cluster-scenarios-high-availability"></a> Cluster High Availability

High availability with Icinga 2 is possible by putting multiple nodes into
a dedicated [zone](#configure-cluster-zones). All nodes will elect one
active master, and retry an election once the current active master is down.

Selected features provide advanced [HA functionality](#high-availability-features).
Checks and notifications are load-balanced between nodes in the high availability
zone.

Connections from other zones will be accepted by all active and passive nodes
but all are forwarded to the current active master dealing with the check results,
commands, etc.

    object Zone "config-ha-master" {
      endpoints = [ "icinga2a", "icinga2b", "icinga2c" ]
    }

Two or more nodes in a high availability setup require an [initial cluster sync](#initial-cluster-sync).

> **Note**
>
> Keep in mind that **only one node acts as configuration master** having the
> configuration files in the `zones.d` directory. All other nodes **must not**
> have that directory populated. Instead they are required to
> [accept synced configuration](#zone-config-sync-permissions).
> Details in the [Configuration Sync Chapter](#cluster-zone-config-sync).

#### <a id="cluster-scenarios-multiple-hierachies"></a> Multiple Hierachies

Your master zone collects all check results for reporting and graphing and also
does some sort of additional notifications.
The customers got their own instances in their local DMZ zones. They are limited to read/write
only their services, but replicate all events back to the master instance.
Within each DMZ there are additional check instances also serving interfaces for local
departments. The customers instances will collect all results, but also send them back to
your master instance.
Additionally the customers instance on the second level in the middle prohibits you from
sending commands to the subjacent department nodes. You're only allowed to receive the
results, and a subset of each customers configuration too.

Your master zone will generate global reports, aggregate alert notifications, and check
additional dependencies (for example, the customers internet uplink and bandwidth usage).

The customers zone instances will only check a subset of local services and delegate the rest
to each department. Even though it acts as configuration master with a master dashboard
for all departments managing their configuration tree which is then deployed to all
department instances. Furthermore the master NOC is able to see what's going on.

The instances in the departments will serve a local interface, and allow the administrators
to reschedule checks or acknowledge problems for their services.


### <a id="high-availability-features"></a> High Availability for Icinga 2 features

All nodes in the same zone require the same features enabled for High Availability (HA)
amongst them.

By default the following features provide advanced HA functionality:

* [Checks](#high-availability-checks) (load balanced, automated failover)
* [Notifications](#high-availability-notifications) (load balanced, automated failover)
* [DB IDO](#high-availability-db-ido) (Run-Once, automated failover)

#### <a id="high-availability-checks"></a> High Availability with Checks

All nodes in the same zone load-balance the check execution. When one instance
fails the other nodes will automatically take over the reamining checks.

> **Note**
>
> If a node should not check anything, disable the `checker` feature explicitely and
> reload Icinga 2.

    # icinga2-disable-feature checker
    # service icinga2 reload

#### <a id="high-availability-notifications"></a> High Availability with Notifications

Notifications are load balanced amongst all nodes in a zone. By default this functionality
is enabled.
If your nodes should notify independent from any other nodes (this will cause
duplicated notifications if not properly handled!), you can set `enable_ha = false`
in the [NotificationComponent](#objecttype-notificationcomponent) feature.

#### <a id="high-availability-db-ido"></a> High Availability with DB IDO

All instances within the same zone (e.g. the `master` zone as HA cluster) must
have the DB IDO feature enabled.

Example DB IDO MySQL:

    # icinga2-enable-feature ido-mysql
    The feature 'ido-mysql' is already enabled.

By default the DB IDO feature only runs on the elected zone master. All other passive
nodes disable the active IDO database connection at runtime.

> **Note**
>
> The DB IDO HA feature can be disabled by setting the `enable_ha` attribute to `false`
> for the [IdoMysqlConnection](#objecttype-idomysqlconnection) or
> [IdoPgsqlConnection](#objecttype-idopgsqlconnection) object on all nodes in the
> same zone.
>
> All endpoints will enable the DB IDO feature then, connect to the configured
> database and dump configuration, status and historical data on their own.

If the instance with the active DB IDO connection dies, the HA functionality will
re-enable the DB IDO connection on the newly elected zone master.

The DB IDO feature will try to determine which cluster endpoint is currently writing
to the database and bail out if another endpoint is active. You can manually verify that
by running the following query:

    icinga=> SELECT status_update_time, endpoint_name FROM icinga_programstatus;
       status_update_time   | endpoint_name
    ------------------------+---------------
     2014-08-15 15:52:26+02 | icinga2a
    (1 Zeile)

This is useful when the cluster connection between endpoints breaks, and prevents
data duplication in split-brain-scenarios. The failover timeout can be set for the
`failover_timeout` attribute, but not lower than 60 seconds.


### <a id="cluster-add-node"></a> Add a new cluster endpoint

These steps are required for integrating a new cluster endpoint:

* generate a new [SSL client certificate](#certificate-authority-certificates)
* identify its location in the zones
* update the `zones.conf` file on each involved node ([endpoint](#configure-cluster-endpoints), [zones](#configure-cluster-zones))
    * a new slave zone node requires updates for the master and slave zones
    * verify if this endpoints requires [configuration synchronisation](#cluster-zone-config-sync) enabled
* if the node requires the existing zone history: [initial cluster sync](#initial-cluster-sync)
* add a [cluster health check](#cluster-health-check)

#### <a id="initial-cluster-sync"></a> Initial Cluster Sync

In order to make sure that all of your cluster nodes have the same state you will
have to pick one of the nodes as your initial "master" and copy its state file
to all the other nodes.

You can find the state file in `/var/lib/icinga2/icinga2.state`. Before copying
the state file you should make sure that all your cluster nodes are properly shut
down.


### <a id="host-multiple-cluster-nodes"></a> Host With Multiple Cluster Nodes

Special scenarios might require multiple cluster nodes running on a single host.
By default Icinga 2 and its features will place their runtime data below the prefix
`LocalStateDir`. By default packages will set that path to `/var`.
You can either set that variable as constant configuration
definition in [icinga2.conf](#icinga2-conf) or pass it as runtime variable to
the Icinga 2 daemon.

    # icinga2 -c /etc/icinga2/node1/icinga2.conf -DLocalStateDir=/opt/node1/var
