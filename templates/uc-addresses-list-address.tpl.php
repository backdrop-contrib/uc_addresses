<?php
/**
 * @file
 * Template file for one address
 */
?>
<table class="list-address <?php print $classes; ?>">
  <tbody>
    <?php if (is_array($fields) && count($fields) > 0): ?>
      <?php foreach ($fields as $field_name => $field): ?>
        <tr class="data-row address-field-<?php print $field_name; ?>">
          <td class="title-col">
            <?php if ($field['title'] != ''):
              print $field['title'] . ':';
            endif; ?>
          </td>
          <td class="data-col"><?php print $field['data']; ?></td>
        </tr>
      <?php endforeach; ?>
    <?php endif; ?>

    <?php if ($admin_links): ?>
      <tr class="address-links">
        <td colspan="2">
          <?php print $admin_links; ?>
        </td>
      </tr>
    <?php endif; ?>
  </tbody>
</table>