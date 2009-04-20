
uc_addresses Module
------------------------
by Tony Freixas

This module was originally authored by Ben Thompson. I've made a lot
of changes since acquiring the module.

The module changes the way Ubercart handles addresses. By default,
Ubercart looks at a customer's previous orders and finds all unique
addresses. It displays these in a select box, using the Street Address
as a label. A customer who has registered but never ordered will have
no contact information other than an e-mail address.

With this module installed, user addresses are stored in a new
database table, one that the user can manipulate as part of the user
profile.

One caveat: The admin order system continues to work as it always did.
Addresses come from previous orders and not from the uc_addresses
table. This trips up a lot of people.

Module overview:
---------------------

When users create an account, you can request that they be asked to
provide contact information. This initial entry can be edited later.

When users visit their "My account" page, a new tab will be present:
Addresses. They will be able to:

  * Add a new address
  * Edit an existing address
  * Mark any one address as the "default"
  * Delete any address except the "default" address

Each address can be given a short "nickname".

When placing an order, users will be able to:

  * Select an address from the set that appears in their profile
  * Modify the address for the order and save it as another address in
    their profile.

At your discretion, the delivery and/or the billing address can be
pre-filled with the user's "default" address.

  Warning: If pre-filling the delivery address and you charge for
  shipping, be sure to require that the user select a shipping method
  before they can place an order. Otherwise, the order may go through
  without shipping being charged. You may also want to use the
  Auto-calculate Shipping module to make things easier for your users.

Instead of selecting an address by street name, the selector will
display the address's nickname or else the entire address: Name,
street1, street2, city, etc.

To accommodate sites that have existing users, the code will work even
when users have no addresses.

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
