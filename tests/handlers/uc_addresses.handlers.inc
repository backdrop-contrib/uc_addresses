<?php
/**
 * @file
 * Contains UcAddressesTextFieldHandler class.
 */

/**
 * This handler extends the handler for a simple text field,
 * adding a check for the field's address type.
 */
class UcAddressesTestTextFieldHandler extends UcAddressesTextFieldHandler {
  /**
   * Checks if field passes the context.
   *
   * @access public
   * @return boolean
   */
  public function checkContext() {
    if (parent::checkContext()) {
      $address = $this->getAddress();
      if (!empty($address->address_type) && $address->address_type != $this->getProperty('address_type')) {
        return FALSE;
      }
      return TRUE;
    }
    return FALSE;
  }
}
