<?php

/**
 * @file
 * Contains uc_addresses_handler_filter_access class.
 */

/**
 * A handler to filter a view by checking address access.
 *
 * Note that this filter does not work well with pager as it alters
 * the results returned by the query and not the query itself.
 * A possible fix for this would be to load all results, but that
 * could become a serious performance hit because:
 * - all addresses must be fully loaded;
 * - load hooks are performed on all loaded addresses;
 * - access hooks are performed on all loaded addresses.
 * Because of that, pager is not supported by this filter.
 *
 * @ingroup views_filter_handlers
 */
class uc_addresses_handler_filter_access extends views_handler_filter_boolean_operator {
  /**
   * Implements views_handler_filter#query().
   *
   * This method needs to be overriden, because else Views
   * will add an extra WHERE condition to the query. This is
   * is not needed as this filter alters the *results*, not
   * the query itself.
   */
  public function query() {
    // Set flag that the access filter is present.
    // This can be handy for other modules to know, for example
    // when they'd like to fix the pager problem.
    $this->view->uc_addresses_filter_access = TRUE;
  }

  /**
   * Loads a single address.
   *
   * @param int $aid
   *   The ID of the address to load.
   * @param int $uid
   *   (optional) The owner of the address.
   *
   * @return UcAddressesAddresss
   *   An address object.
   */
  public function loadAddress($aid, $uid = NULL) {
    if (empty($uid)) {
      $address = UcAddressesAddressBook::loadAddress($aid);
    }
    else {
      $address = UcAddressesAddressBook::get($uid)->getAddressById($aid);
    }
    return $address;
  }

  /**
   * Removes the rows the user should have no access to.
   *
   * @param array $values
   *   The loaded values.
   *
   * @todo This doesn't work with pager.
   */
  public function post_execute(&$values) {
    // Guess the field name of 'aid' in the results.
    if (!empty($this->relationship)) {
      // Used table is assumed to be the relationship.
      $table = $this->relationship;
    }
    else {
      $table = 'uc_addresses';
    }
    $alias = $this->view->query->field_aliases[$table]['aid'];

    foreach ($values as $index => $row) {
      $aid = $row->$alias;
      $address = $this->loadAddress($aid);
      $uid = $address->getUserId();
      $address_user = user_load($uid);
      // Check for access.
      if (
        ($this->value && !$this->check_access($address_user, $address))
        || (!$this->value && $this->check_access($address_user, $address))
      ) {
        // No access. Remove the row from the results and decrease the calculated
        // number of rows.
        unset($values[$index]);
        $this->view->total_rows--;
      }
    }
  }

  /**
   * Checks address access for the current user.
   *
   * @param object $address_user
   *   The user who owns the address.
   * @param UcAddressesAddress
   *   The address to check access for.
   *
   * @return boolean
   *   TRUE, if the user should have access.
   *   FALSE otherwise.
   */
  public function check_access($address_user, $address) {
    switch ($this->definition['uc_addresses_access_type']) {
      case 'view':
        return UcAddressesPermissions::canViewAddress($address_user, $address);
      case 'edit':
        return UcAddressesPermissions::canEditAddress($address_user, $address);
      case 'delete':
        return UcAddressesPermissions::canDeleteAddress($address_user, $address);
    }
  }
}
