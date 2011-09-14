<?php
/**
 * @file
 * Template file for address book, listing all addresses
 */
?>
<div class="address-book">
  <?php if (count($addresses) > 0): ?>
    <?php if ($default_billing_address || $default_shipping_address): ?>
      <!-- Default addresses -->
      <div class="default-addresses">
        <h2><?php print t('Default addresses'); ?></h2>
        <ol>
        <?php if ($default_billing_address): ?>
          <li class="address-item">
            <h3><?php print t('Default billing address'); ?></h3>
            <?php print $default_billing_address; ?>
          </li>
        <?php endif; ?>
        <?php if ($default_shipping_address): ?>
          <li class="address-item">
            <h3><?php print t('Default shipping address'); ?></h3>
            <?php print $default_shipping_address; ?>
          </li>
        <?php endif; ?>
        </ol>
      </div>
      <!-- Other addresses -->
      <?php if (count($other_addresses) > 0): ?>
        <div class="additional-addresses">
          <h2><?php print t('Other addresses'); ?></h2>
          <ol>
          <?php foreach ($other_addresses as $aid => $address): ?>
            <li class="address-item">
              <?php print $address; ?>
            </li>
          <?php endforeach; ?>
          </ol>
        </div>
      <?php endif; ?>
    <?php else: ?>
      <?php if (count($other_addresses) > 0): ?>
        <!-- All addresses -->
        <div class="addresses">
          <h2><?php print t('Addresses'); ?></h2>
          <ol>
          <?php foreach ($other_addresses as $aid => $address): ?>
            <li class="address-item">
              <?php print $address; ?>
            </li>
          <?php endforeach; ?>
          </ol>
        </div>
      <?php endif; ?>
    <?php endif; ?>
  <?php else: ?>
    <?php print t('No addresses have been saved.'); ?>
  <?php endif; ?>

  <?php if ($add_address_link): ?>
    <div class="address-links">
      <?php print $add_address_link; ?>
    </div>
  <?php endif; ?>
</div>