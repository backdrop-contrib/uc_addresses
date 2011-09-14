<?php
/**
 * @file
 * Ubercart callbacks for the checkout- and order panes
 */

/**
 * Callback for the billing pane
 *
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the checkout form
 * @return mixed
 */
function uc_addresses_checkout_pane_billing($op, &$order, $values = array()) {
  return uc_addresses_checkout_pane_address('billing', $op, $order, $values);
}

/**
 * Callback for the delivery pane
 *
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the checkout form
 * @return mixed
 */
function uc_addresses_checkout_pane_shipping($op, &$order, $values = array()) {
  return uc_addresses_checkout_pane_address('shipping', $op, $order, $values);
}

/**
 * Callback for the delivery pane
 *
 * @param string $type
 *   The address type: billing or delivery
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the checkout form
 *
 * @return mixed
 */
function uc_addresses_checkout_pane_address($type, $op, &$order, $values = array()) {
  global $user;
  drupal_add_js(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.js');

  $other_type = ($type == 'shipping')? 'billing':'shipping';
  $copy_address_title = ($type == 'shipping')? t('My delivery information is the same as my billing information.'):t('My billing information is the same as my delivery information.');

  switch ($op) {
    case 'view':
      // Tell addres book we want to get multiple addresses
      UcAddressesAddressBook::get($user->uid)->setPerformanceHint(UcAddressesAddressBook::PERF_HINT_LOAD_ALL);

      // Initialize address variable
      $address = NULL;

      // Initialize pane description
      switch ($type) {
        case 'shipping':
          $description = t('Enter your delivery address and information here.');
          break;
        case 'billing':
          $description = t('Enter your billing address and information here.');
          break;
      }

      // Check if address may be automatically filled in
      $auto_fill_address = (variable_get('uc_addresses_default_' . $type . '_address', TRUE) && variable_get('uc_addresses_use_default_' . $type, TRUE));

      // Get address for order if set
      if (isset($order->uc_addresses[$type])) {
        $address = $order->uc_addresses[$type];
      }
      elseif ($auto_fill_address) {
        // Get default address of type $type
        $address = UcAddressesAddressBook::get($user->uid)->getDefaultAddress($type);
        if ($address) {
          $address = $address->copyAddress(UcAddressesAddressBook::get(0));
        }
      }

      // View the address form
      $form[$type] = array(
        '#type' => 'uc_addresses_address',
        '#uc_addresses_context' => 'checkout_form',
      );
      if (!$address) {
        $address = UcAddressesAddress::newAddress();
      }
      // Add address to the form
      $form[$type]['#uc_addresses_address'] = $address;
      $form['uc_addresses_address'] = array(
        '#type' => 'value',
        '#value' => $address,
      );

      // Copy address checkbox
      if ((uc_cart_is_shippable() || !variable_get('uc_cart_delivery_not_shippable', TRUE)) &&
          _checkout_pane_data('uc_addresses_' . $other_type, 'weight') < _checkout_pane_data('uc_addresses_' . $type, 'weight') &&
          _checkout_pane_data('uc_addresses_' . $other_type, 'enabled')) {
        $form['copy_address'] = array(
          '#type' => 'checkbox',
          '#title' => $copy_address_title,
          '#attributes' => array(
            'class' => 'uc_addresses_copy_address',
            'onclick' => "uc_addresses_copy_address(this.checked, '$other_type', '$type');",
          ),
          '#weight' => -5,
          '#default_value' => ($address->copy_address)? TRUE:FALSE,
        );
      }

      // Address book selecting
      $addresses = UcAddressesAddressBook::get($user->uid)->getAddresses();
      $has_saved_addresses = FALSE;
      if (count($addresses) > 0) {
        // Check if there is at least one saved addresses
        foreach ($addresses as $address) {
          if (!$address->isNew()) {
            $has_saved_addresses = TRUE;
            break;
          }
        }
      }
      // Only display saved addresses dropdown if the user has at least one saved address
      if ($has_saved_addresses) {
        $description =
          (
            ($auto_fill_address)?
            t('Edit the address below or select an address from the list.') :
            t('Enter an address below or select an address from the list.')
          )
          . ' '
          . t('Go to your <a href="@address-book">address book</a> to manage your saved addresses.', array('@address-book' => url('user/'. $user->uid . '/addresses')));

        $address_book_default_value = NULL;
        // Display a way of selecting an address
        $options = array(
          '0' => t('Select one...'),
        );
        foreach ($addresses as $address) {
          // Display saved addresses only
          if ($address->isNew()) {
            continue;
          }
          $data = array();
          $fields = $address->getRawFieldData();
          // Replace underscores and spaces with a hyphen
          foreach ($fields as $fieldname => $value) {
            $fieldname_with_hyphens = str_replace(array('_', ' '), '-', $fieldname);
            $data[$fieldname_with_hyphens] = $value;
          }
          if ($address_name = $address->getName()) {
            $options[drupal_to_js($data)] = $address_name;
          }
          else {
            $options[drupal_to_js($data)] = preg_replace('/<.*?>/', ', ', uc_addresses_format_address($address));
          }

          // Add default value if address was automatically filled
          if ($address->compareAddress($form['uc_addresses_address']['#value'])) {
            $address_book_default_value = drupal_to_js($data);
          }
        }

        $form[$type]['addressbook'] = array(
          '#type' => 'select',
          '#title' => t('Saved addresses'),
          '#attributes' => array('onchange' => "uc_addresses_apply_address('" . $type . "', this.value);"),
          '#options' => $options,
          '#weight' => -50,
        );
        // Add default value for address book
        if ($address_book_default_value) {
          $form[$type]['addressbook']['#default_value'] = $address_book_default_value;
        }
      }

      return array(
        'description' => $description,
        'contents' => $form,
      );
      break;

    case 'process':
      $order_type = $type;
      if ($type == 'shipping') {
        $order_type = 'delivery';
      }
      foreach ($values[$type] as $fieldname => $value) {
        $order->{$order_type . '_' . $fieldname} = $value;
      }

      $address = $values['uc_addresses_address'];
      if ($address->isNew() && !(isset($values['copy_address']) && $values['copy_address'])) {
        // Set flag that this address may be saved to the addressbook
        $address->save_address = TRUE;
      }
      if (isset($values['copy_address']) && $values['copy_address']) {
        $address->copy_address = TRUE;
      }
      else {
        $address->copy_address = FALSE;
      }
      $order->uc_addresses[$type] = $address;

      // Save address into session
      $_SESSION['uc_addresses_order'][$type] = serialize($address);
      return TRUE;

    case 'review':
      drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
      return uc_addresses_preprocess_address($order->uc_addresses[$type], 'checkout_review');
  }
}
