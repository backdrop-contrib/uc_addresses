<?php
/**
 * @file
 * Ubercart callbacks for the checkout- and order panes
 */

// ---------------------------------------------------------------------------
// UBERCART CALLBACKS
// checkout panes
// ---------------------------------------------------------------------------

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
 * Callback for an address checkout pane
 *
 * @param string $type
 *   The address type: billing or shipping
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the checkout form
 * @return mixed
 */
function uc_addresses_checkout_pane_address($type, $op, &$order, $values = array()) {
  global $user;
  drupal_add_js(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.js');

  if ($type == 'shipping') {
    $other_type = 'billing';
    $uc_type = 'delivery';
  }
  else {
    $other_type = 'delivery';
    $uc_type = 'billing';
  }
  $copy_address_title = ($type == 'shipping') ? t('My delivery information is the same as my billing information.') : t('My billing information is the same as my delivery information.');

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
      $form[$uc_type] = array(
        '#type' => 'uc_addresses_address',
        '#uc_addresses_context' => 'checkout_form',
        '#key_prefix' => $uc_type,
      );
      if (!$address) {
        $address = UcAddressesAddress::newAddress();
      }
      // Add address to the form
      $form[$uc_type]['#uc_addresses_address'] = $address;
      $form['uc_addresses_address'] = array(
        '#type' => 'value',
        '#value' => $address,
      );

      // Copy address checkbox
      if ((uc_cart_is_shippable() || !variable_get('uc_cart_delivery_not_shippable', TRUE)) &&
          _checkout_pane_data($other_type, 'weight') < _checkout_pane_data($uc_type, 'weight') &&
          _checkout_pane_data($other_type, 'enabled')) {
        $form['copy_address'] = array(
          '#type' => 'checkbox',
          '#title' => $copy_address_title,
          '#attributes' => array(
            'class' => 'uc_addresses_copy_address',
            'onclick' => "uc_addresses_copy_address_checkout(this.checked, '$other_type', '$uc_type');",
          ),
          '#weight' => -5,
          '#default_value' => ($address->copy_address) ? TRUE : FALSE,
        );
      }

      // Address book selecting
      if ($select = uc_addresses_select_address($user->uid, 'checkout_form', $uc_type, $form['uc_addresses_address']['#value'])) {
        $description =
          (
            ($auto_fill_address) ?
            t('Edit the address below or select an address from the list.') :
            t('Enter an address below or select an address from the list.')
          )
          . ' '
          . t('Go to your <a href="@address-book">address book</a> to manage your saved addresses.', array('@address-book' => url('user/' . $user->uid . '/addresses')))
        ;
        $form['addresses']['#title'] = t('Saved addresses');
        $form[$uc_type]['addressbook'] = $select;
      }

      return array(
        'description' => $description,
        'contents' => $form,
      );
      break;

    case 'process':
      $uc_type = $type;
      if ($type == 'shipping') {
        $uc_type = 'delivery';
      }

      foreach ($values[$uc_type] as $fieldname => $value) {
        $order->{$fieldname} = $value;
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
      $_SESSION['uc_addresses_order'][$order->order_id][$type] = serialize($address);
      return TRUE;

    case 'review':
      drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
      return uc_addresses_preprocess_address($order->uc_addresses[$type], 'checkout_review');
  }
}

// ---------------------------------------------------------------------------
// UBERCART CALLBACKS
// order panes
// ---------------------------------------------------------------------------

/**
 * Callback for the "bill to" pane
 *
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the order form
 * @return mixed
 */
function uc_addresses_order_pane_bill_to($op, $order, $values = array()) {
  return uc_addresses_order_pane_address('billing', $op, $order, $values);
}

/**
 * Callback for the "ship to" pane
 *
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the order form
 * @return mixed
 */
function uc_addresses_order_pane_ship_to($op, $order, $values = array()) {
  return uc_addresses_order_pane_address('shipping', $op, $order, $values);
}

/**
 * Callback for an address order pane
 *
 * @param string $type
 *   The address type: billing or shipping
 * @param string $op
 *   The operation that is being performed
 * @param array/object $order
 *   The Ubercart order
 * @param array $values
 *   The values from the order form
 * @return mixed
 */
function uc_addresses_order_pane_address($type, $op, $order, $values = array()) {
  global $user;
  drupal_add_js(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.js');
  drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.js');

  if ($type == 'shipping') {
    $other_type = 'billing';
    $uc_type = 'delivery';
  }
  else {
    $other_type = 'delivery';
    $uc_type = 'billing';
  }

  switch ($op) {
    case 'customer':
    case 'view':
      drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
      $lines = array();
      $values = uc_addresses_preprocess_address($order->uc_addresses[$type], 'order_view');
      foreach ($values as $fieldname => $value) {
        if (isset($value['title']) && $value['title'] != '') {
          $lines[] = '<strong>' . $value['title'] . '</strong>: ' . $value['data'];
        }
        else {
          $lines[] = $value['data'] . '<br />';
        }
      }
      $output = '<br />' . implode('<br />', $lines);
      return $output;

    case 'edit-form':
      // Initialize pane description
      switch ($type) {
        case 'shipping':
          $description = t("Modify 'Ship to' information");
          break;
        case 'billing':
          $description = t("Modify 'Bill to' information");
          break;
      }

      // Get address for order if set
      if (isset($order->uc_addresses[$type])) {
        $address = $order->uc_addresses[$type];
      }

      $form = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'Bill to' information"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      // View the address form
      $form[$uc_type] = array(
        '#type' => 'uc_addresses_address',
        '#uc_addresses_context' => 'order_form',
        '#key_prefix' => $uc_type,
      );
      if (!$address) {
        $address = UcAddressesAddress::newAddress();
      }
      // Add address to the form
      $form[$uc_type]['#uc_addresses_address'] = $address;
      return $form;

    case 'edit-title':
      switch ($type) {
        case 'shipping':
          $copy_text = t("Copy billing information.");
          break;
        case 'billing':
          $copy_text = t("Copy shipping information.");
          break;
      }
      $output = ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
        . '/images/address_book.gif" alt="' . t('Select from address book.') . '" '
        . 'title="' . t('Select from address book.') . '" onclick="load_address_select(' . $order['order_uid']['#value'] . ', \'#' . $uc_type . '_address_select\', \'' . $uc_type . '\');" '
        . 'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
        . '/images/copy.gif" alt="' . $copy_text . '" title="'
        . $copy_text . '" onclick="uc_addresses_copy_address_order(\'' . $other_type . '\', \'' . $uc_type . '\');" '
        . 'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      $output = '<div id="' . $uc_type . '-pane"><div id="' . $uc_type . '_address_select"></div>' . drupal_render($order[$uc_type]) . '</div>';
      return $output;

    case 'edit-process':
      $order_values = $order;
      $order = uc_order_load($order_values['order_id']);
      $address = $order->uc_addresses[$type];
      $changes = array();

      foreach ($order_values[$uc_type] as $key => $value) {
        // Set value
        $order_values[$key] = $value;

        // Check if the value was changed
        $fieldname = substr($key, strlen($uc_type) + 1);
        try {
          if ($address->getField($fieldname) != $value) {
            $changes[$key] = $value;
          }
        }
        catch (UcAddressesException $e) {
          // Ignore any Ubercart Addresses exceptions.
        }
      }
      return $changes;
  }
}
