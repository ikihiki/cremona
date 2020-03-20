driver
device_create_file -> cremona/showname/toot
module_param -> port

toot open
driver - open toot (netlink) -> daemon

toot write 
app - data (file) -> driver - data (netlink) -> demon

toot flush
driver - flush (netlink) -> daemon


driver
netlink.c
create_link
