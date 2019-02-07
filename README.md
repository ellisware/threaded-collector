# Data Collector Template

Data collection is a common task.  This is just a generic template to ingress data via a source such as a proprietary c++ device API, then make the data available via any number of standardized protocols (MQTT, Websocket, OPC, etc)

Overall Archetecture:

Ingress Thread (Data Collection) -> Main Thread (Data Store)  -> Egress Threads (Data Consumer)


Note: Requires Boost Packages:

PM> Install-Package boost
PM> Install-Package boost_date_time-vc141
PM> Install-Package boost_filesystem-vc141
PM> Install-Package boost_log-vc141
PM> Install-Package boost_system-vc141
PM> Install-Package boost_thread-vc141
PM> Install-Package boost_chrono-vc141
PM> Install-Package boost_log_setup-vc141
PM> Install-Package boost_atomic-vc141
