
uc_addresses Module
------------------------
by Ben Thompson
modified by Tony Freixas

Most of the code in this module was written by Ben Thompson. I (Tony
Freixas) adapted the code, giving it a rather different mode of
operation.

The module changes the way Ubercart handles addresses. By default,
Ubercart looks at a customer's previous orders and finds all unique
addresses. It displays these in a select box, using the Street Address
as a label. A customer who has registered but never ordered will have
no contact information other than an e-mail address.

With this module installed, user addresses are stored in a new
database table, one that the user can manipulate as part of the user profile.

Module overview:
---------------------

When users create an account, they will be asked to provide contact
information. This initial entry can be edited later, but not deleted.

When users visit their "My account" page, a new tab will be present:
Addresses. They will be able to:

  * Add a new address
  * Edit an existing address
  * Delete any address except the first one

When placing an order, users will be able to:

  * Select an address from the set that appears in their profile
  * Modify the address, but just for the order
  * Modify the address for the order and save it as another address in
    their profile.

Instead of selecting an address by street name, the selector will
display the entire address: Name, street1, street2, city, etc.

Unlike core Ubercart and unlike Ben's original code, the orders are
never searched for addresses.

To accommodate sites that have existing users, the code will work even
when users have no addresses. Once they save the first address, it
will become their default contact information and they won't be
allowed to delete it.

Note: when a user is deleted, all their address are also deleted.

Dependencies
------------

This module requires uc_order and uc_store.

Installation
------------

  * Copy the uc_addresses module's directory to your modules directory
    and activate it. I have mine in /sites/all/modules/uc_addresses.
  * Activate the module, set up permissions and go to your account
    page to begin using the new Addresses tab.

If you installed a version of Ben's module, you will need to disable
and uninstall it. I've kept the same database name, but the fields are
slightly different.

Permissions
-----------

view default addresses: Roles with this permission can view anyone's
default address.

view all addresses: Roles with this permission can view anyone's list
of addresses.

add/edit addresses: Roles with this permission can add to or edit
anyone's address list. To be useful, roles with this permission can
also view anyone's list of addresses.

To be complete, I should add 'view own default address', 'view own
addresses', and 'add/edit own addresses', but I can't see that any of
these make sense.

Tony Freixas
tony@tigerheron.com
http://www.tigerheron.com
