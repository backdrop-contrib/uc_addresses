<?php
/**
 * @file
 * Permission class
 */

/**
 * The permission class: UcAddressesPermissions
 *
 * This class checks for view, edit and delete access for a single address. So
 * whenever you want to check permissions for address access, you should not call
 * user_access(), but call the appropiate method in this class. Call
 * canViewAddress() for view access, canEditAddress() for edit access and
 * canDeleteAddress() for delete access. The class will then take care for calling
 * user_access() itself.
 *
 * If the permissions defined by Ubercart Addresses do not fit your needs, you are
 * able to get further control about address access by implementing
 * hook_uc_addresses_may_view(), hook_uc_addresses_may_edit() or
 * hook_uc_addresses_may_delete(). See uc_addresses.api.php - included with the
 * module - for more information.
 */
class UcAddressesPermissions {
  // -----------------------------------------------------------------------------
  // CONSTANTS
  // -----------------------------------------------------------------------------

  /**
   * Give users permissiony to view their default address
   */
  const VIEW_OWN_DEFAULT = 'view own default addresses';

  /**
   * Give users permission to view all of their own addresses
   * Implies VIEW_OWN_DEFAULT.
   */
  const VIEW_OWN = 'view own addresses';

  /**
   * Give users the ability to view anyone's default address.
   * Implies VIEW_OWN_DEFAULT.
   */
  const VIEW_ALL_DEFAULTS = 'view all default addresses';

  /**
   * Give users the ability to view anyone's addresses.
   * Implies VIEW_OWN, VIEW_ALL_DEFAULTS.
   */
  const VIEW_ALL = 'view all addresses';

  /**
   * Give users the ability to add or edit their own addresses.
   * Implies VIEW_OWN.
   */
  const EDIT_OWN = 'add/edit own addresses';

  /**
   * Give users the ability to add or edit anyone's addresses.
   * Implies VIEW_ALL, EDIT_OWN.
   */
  const EDIT_ALL = 'add/edit all addresses';

  /**
   * Give users the ability to delete their own addresses.
   * Implies VIEW_OWN.
   */
  const DELETE_OWN = 'delete own addresses';

  /**
   * Give users the ability to delete anyone's addresses.
   * Implies VIEW_ALL, DELETE_OWN.
   */
  const DELETE_ALL = 'delete all addresses';

  // -----------------------------------------------------------------------------
  // METHODS
  // -----------------------------------------------------------------------------

  /**
   * Check if user may view this address
   *
   * @param object $address_user
   *   User object
   * @param UcAddressesAddress|NULL $address
   *   (optional) The address object
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canViewAddress($address_user, UcAddressesAddress $address = NULL) {
    global $user;

    if ($address_user->uid == $user->uid) {
      // User is the owner of the address

      // If trying to view own address
      if (self::canViewOwn()) {
        // Ask other modules if the address may be viewed.
        return self::invoke('uc_addresses_may_view', $address_user, $address);
      }

      // If viewing all addresses, we permit the operation if the user
      // can view the default address. The non-default addresses will
      // need to be filtered out elsewhere.
      if ($address == NULL) {
        if (self::canViewOwnDefault()) {
          // Ask other modules if the address may be viewed.
          return self::invoke('uc_addresses_may_view', $address_user, $address);
        }
        return FALSE;
      }

      // Check if the address is a default address and if the user
      // may view own default addresses.
      if ($address->isDefault('shipping') || $address->isDefault('billing')) {
        if (self::canViewOwnDefault()) {
          // Ask other modules if the address may be viewed.
          return self::invoke('uc_addresses_may_view', $address_user, $address);
        }
      }
    }
    else {
      // User is NOT the owner of the address

      // If trying to view someone else's address
      if (self::canViewAll()) {
        // Ask other modules if the address may be viewed.
        return self::invoke('uc_addresses_may_view', $address_user, $address);
      }

      // If viewing all addresses, we permit the operation if the user
      // can view the default address. The non-default addresses will
      // need to be filtered out elsewhere.
      if ($address == NULL) {
        if (self::canViewAllDefaults()) {
          // Ask other modules if the address may be viewed.
          return self::invoke('uc_addresses_may_view', $address_user, $address);
        }
        return FALSE;
      }

      // Check if the address is a default address and if the user
      // may view default addresses of all users.
      if ($address->isDefault('shipping') || $address->isDefault('billing')) {
        if (self::canViewAllDefaults()) {
          // Ask other modules if the address may be viewed.
          return self::invoke('uc_addresses_may_view', $address_user, $address);
        }
      }
    }

    // No other cases are permitted
    return FALSE;
  }

  /**
   * Check if the user can edit addresses of this user
   *
   * @param object $address_user
   *   User object
   * @param UcAddressesAddress
   *   The address object, optional
   * @access public
   * @static
   * @return boolean
   */
  static public function canEditAddress($address_user, UcAddressesAddress $address = NULL) {
    global $user;

    if ($address_user->uid == $user->uid && self::canEditOwn()) {
      // Ask other modules if the address may be edited.
      return self::invoke('uc_addresses_may_edit', $address_user, $address);
    }
    if ($address_user->uid != $user->uid && self::canEditAll()) {
      // Ask other modules if the address may be edited.
      return self::invoke('uc_addresses_may_edit', $address_user, $address);
    }

    // No other cases are permitted
    return FALSE;
  }

  /**
   * Check if the user can delete addresses of this user.
   * Default addresses can never be deleted.
   *
   * @param object $address_user
   *   User object
   * @param UcAddressesAddress
   *   The address object, optional
   * @access public
   * @static
   * @return boolean
   */
  static public function canDeleteAddress($address_user, UcAddressesAddress $address = NULL) {
    global $user;

    if ($address instanceof UcAddressesAddress) {
      // Check if the address is a default address. If so, the address may not be deleted.
      if ($address->isDefault('shipping') || $address->isDefault('billing')) {
        return FALSE;
      }
    }

    if ($address_user->uid == $user->uid && self::canDeleteOwn()) {
      // Ask other modules if the address may be deleted.
      return self::invoke('uc_addresses_may_delete', $address_user, $address);
    }
    if ($address_user->uid != $user->uid && self::canDeleteAll()) {
      // Ask other modules if the address may be deleted.
      return self::invoke('uc_addresses_may_delete', $address_user, $address);
    }

    // No other cases are permitted
    return FALSE;
  }

  /**
   * If the logged in user may view it's own default address
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canViewOwnDefault() {
    return
      user_access(self::VIEW_OWN_DEFAULT) ||
      self::canViewAllDefaults() ||
      self::canViewOwn();
  }

  /**
   * If the logged in user may view it's own addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canViewOwn() {
    return
      user_access(self::VIEW_OWN) ||
      self::canViewAll() ||
      self::canEditOwn() ||
      self::canDeleteOwn();
  }

  /**
   * If the logged in user may view all default addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canViewAllDefaults() {
    return
      user_access(self::VIEW_ALL_DEFAULTS) ||
      self::canViewAll();
  }

  /**
   * If the logged in user may view all addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canViewAll() {
    return
      user_access(self::VIEW_ALL) ||
      self::canEditAll() ||
      self::canDeleteAll();
  }

  /**
   * If the logged in user may edit own addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canEditOwn() {
    return
      user_access(self::EDIT_OWN) ||
      self::canEditAll();
  }

  /**
   * If the logged in user may edit all addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canEditAll() {
    return
      user_access(self::EDIT_ALL);
  }

  /**
   * If the logged in user may delete own addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canDeleteOwn() {
    return
      user_access(self::DELETE_OWN) ||
      self::canDeleteAll();
  }

  /**
   * If the logged in user may delete all addresses
   *
   * @access public
   * @static
   * @return boolean
   */
  static public function canDeleteAll() {
    return
      user_access(self::DELETE_ALL);
  }

  // -----------------------------------------------------------------------------
  // PRIVATE
  // -----------------------------------------------------------------------------

  /**
   * Ask other modules if a particular operation is permitted.
   *
   * @param string $hook
   *   The hook to invoke.
   * @param object $address_user
   *   User object
   * @param UcAddressesAddress
   *   The address object, optional
   * @access private
   * @static
   * @return boolean
   */
  static private function invoke($hook, $address_user, UcAddressesAddress $address = NULL) {
    global $user;
    if ($user->uid != 1) {
      // Ask other modules if the operation on the address is permitted.
      // If one of the modules returns FALSE, then the operation on the address is not permitted.
      // The superuser (user 1) may do everything, for this user the check is bypassed.
      foreach (module_implements($hook) as $module) {
        $function = $module . '_' . $hook;
        if (!$function($address_user, $address)) {
          return FALSE;
        }
      }
    }
    return TRUE;
  }
}
