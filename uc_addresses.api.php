<?php
/**
 * @file
 * These hooks are invoked by the Ubercart Addresses module.
 * @todo more documentation needed for hook_uc_addresses_field_handlers().
 * @todo Document the rest of the API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * With this hook you can register new field handlers that can be
 * used with address edit forms.
 *
 * A field handler is a class that extends UcAddressesFormFieldHandler.
 *
 * @return array
 */
function hook_uc_addresses_field_handlers() {
  $path = drupal_get_path('module', 'mymodule') .'/handlers';

  $info = array();
  $info['MyCustomFieldHandler'] = array(
    'hidden' => FALSE,
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'MyCustomFieldHandler',
      'file' => 'MyCustomFieldHandler.class.php',
      'path' => $path,
    ),
  );
}

/**
 * This hook allows you to register extra fields to be used in
 * all address edit forms.
 *
 * Format to return:
 * Array (
 *   fieldname => Array (
 *     'handler' => handler class (registered through hook_uc_addresses_field_handlers())
 *     'display_settings' (array of contexts to show or hide the field on)
 *     'strings' (array of strings to be used in the field handler)
 *   ),
 * )
 *
 * Adding display settings to the field definition is optional.
 * If you don't set this, assumed is that the field may be showed
 * everywhere.
 *
 * Define the strings array only if you have multiple fields using
 * the same handler, but with slighty different texts.
 *
 * @return array
 */
function hook_uc_addresses_fields() {
  // Example: register my own field
  return array(
    'myfield' => array(
      'handler' => 'MyCustomFieldHandler',
      'display_settings' => array(
        'default' => TRUE, // Display it by default
        'address_form' => TRUE, // Display it on the address edit form
        'address_view' => TRUE, // Display it in the address book
        'checkout_form' => FALSE, // Don't display during checkout
        'checkout_review' => FALSE, // Don't display at checkout review
        'order_form' => TRUE, // Display on order edit forms
        'order_view' => TRUE, // Display on order pages
      ),
      'strings' => array(),
    );
  );
}

/**
 * When all modules have registered their fields, you have
 * a chance to alter the definitions with this hook.
 *
 * @param array $fields
 * @return void
 */
function hook_uc_addresses_fields_alter(&$fields) {
  // Change the handler of my custom field
  $fields['myfield']['handler'] = 'MyOtherCustomFieldHandler';
}

/**
 * This hook allows to you alter a uc_addresses_address field element.
 *
 * This is useful if you want to make a change to address edit forms
 * that's applyable for all places it appears.
 *
 * The address object the field element is for can be find in
 * $element['#uc_addresses_address']
 *
 * @param array $element
 *   The form element, contains several subfields.
 * @return void
 */
function hook_uc_addresses_address_field_alter(&$element) {
  // Add extra validation if the address of this element is a default billing address
  $address = $element['#uc_addresses_address'];
  if ($address->isDefault('billing')) {
    $element['#element_validate'][] = 'mymodule_uc_addresses_address_validate';
  }
}

/**
 * This hook allows you to alter the address field listing before
 * it's being displayed.
 *
 * Examples of where the addresses can be listed:
 * - the address book
 * - the checkout review page
 * - order view pages
 *
 * @param array $fields
 *   An array containing the field data like this:
 *   Array (
 *     fieldname => Array (
 *       'title' => field title (string)
 *       'data' => field value (string)
 *       '#weight' => weight (int)
 *     )
 *   )
 * @param UcAddressesAddress $address
 *   The address object
 * @param string $context
 *   The context in which the fields will be displayed:
 *   - 'address_view' => the address book
 *   - 'checkout_review' => the checkout review page
 *   - 'order_view' => order view pages
 * @return void
 */
function hook_uc_addresses_preprocess_address_alter(&$fields, $address, $context) {
  // Example 1: add extra data in case this is the default shipping address
  if ($address->isDefault('shipping')) {
    $fields['mydata'] = array(
      'title' => t('Title'),
      'data' => t('Default shipping address'),
      '#weight' => 1,
    );
  }

  // Example 2: move my own field to the top.
  if (isset($fields['myfield'])) {
    $fields['myfield']['#weight'] = -20;
  }
}

/**
 * This hook allows you to act on addresses being loaded from the database.
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @param object $obj
 *   The fetched database record
 * @return void
 */
function hook_uc_addresses_address_load($address, $obj) {
  // Example: set a value for my custom added field (through hook_uc_addresses_fields())
  $address->setField('myfield', 'myvalue');
}

/**
 * This hook allows you alter the address just before it's saved.
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @return void
 */
function hook_uc_addresses_address_presave($address) {
  // Example: set a nickname for this address if there is none.
  if ($address->getName() == '') {
    $nickname = 'my address name';
    if (!$address->setName($nickname)) {
      // Try an other name if this nickname is already used.
      $numb = 2;
      $other_nickname = $nickname . ' ' . $numb++;
      while (!$address->setName($other_nickname)) {
        $other_nickname = $nickname . ' ' . $numb++;
      }
    }
  }
}

/**
 * This hook allows you to respond to creation of a new address.
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @return void
 */
function hook_uc_addresses_address_insert($address) {
  // Example: get the value of my custom field and insert it in my own table
  $record = array(
    'aid' => $address->getId(),
    'myfield' => $address->getField('myfield'),
  );
  drupal_write_record('mydbtable', $record);
}

/**
 * This hook allows you to respond to updates to an address.
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @return void
 */
function hook_uc_addresses_address_update($address) {
  // Example: get the value of my custom field and update it in my own table
  $record = array(
    'aid' => $address->getId(),
    'myfield' => $address->getField('myfield'),
  );
  drupal_write_record('mydbtable', $record, array('aid'));
}

/**
 * This hook allows you to respond to address deletion.
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @return void
 */
function hook_uc_addresses_address_delete($address) {
  // Example: delete the value from my table
  db_query('DELETE FROM {mydbtable} WHERE aid = %d', $address->getId());
}

/**
 * This hooks allows you to prevent a certain address from being viewed.
 *
 * Don't use this hook if you want to prevent viewing addresses for users
 * with a certain role. You can use the permission settings for that.
 *
 * If you want the address not to be viewed return FALSE.
 * Return TRUE in all other cases.
 * WARNING: If you don't return TRUE, then no address may be viewed.
 *
 * Note that this hook is only invoked when permissions are checked and not
 * when the address itself is displayed (e.g. through theme('uc_addresses_list_address')).
 *
 * @param object $address_user
 *   Owner of the address
 * @param UcAddressesAddress $address
 *   Address object, optional
 * @return boolean
 */
function hook_uc_addresses_may_view($address_user, $address) {
  // No specific restrictions for viewing addresses
  return TRUE;
}

/**
 * This hooks allows you to prevent a certain address from being edited.
 *
 * Don't use this hook if you want to prevent editing addresses for users
 * with a certain role. You can use the permission settings for that.
 *
 * If you want the address not to be edited return FALSE.
 * Return TRUE in all other cases.
 * WARNING: If you don't return TRUE, then no address may be edited.
 *
 * Note that this hook is only invoked when permissions are checked and not
 * when changes to an address are done programmatically.
 *
 * @param object $address_user
 *   Owner of the address
 * @param UcAddressesAddress $address
 *   Address object, optional
 * @return boolean
 */
function hook_uc_addresses_may_edit($address_user, $address) {
  // Example: don't allow editing of default addresses.
  if ($address instanceof UcAddressesAddress) {
    if ($address->isDefault('shipping') || $address->isDefault('billing')) {
      return FALSE;
    }
  }
  // In all other cases, the address may be edited.
  return TRUE;
}

/**
 * This hooks allows you to prevent a certain address from being deleted.
 *
 * Don't use this hook if you want to prevent deleting addresses for users
 * with a certain role. You can use the permission settings for that.
 * Default addresses are always protected from being deleted.
 *
 * If you want the address not to be deleted return FALSE.
 * Return TRUE in all other cases.
 * WARNING: If you don't return TRUE, then no address may be deleted.
 *
 * Note that this hook is only invoked when permissions are checked and not
 * when an address is deleted programmatically.
 *
 * @param object $address_user
 *   Owner of the address
 * @param UcAddressesAddress $address
 *   Address object, optional
 * @return boolean
 */
function hook_uc_addresses_may_delete($address_user, $address) {
  // No specific restrictions for deleting addresses
  return TRUE;
}

/**
 * @} End of "addtogroup hooks".
 */
