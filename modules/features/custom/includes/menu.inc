<?php

/**
 * @file
 * Functions for Menu module.
 */

/**
 * Implements hook_menu().
 */
function custom_menu() {
  $items = array();

  // Custom book items.
  $items += custom_book_menu();

  return $items;
}

/**
 * Implements hook_menu_alter().
 */
function custom_menu_alter(&$items) {
  // Custom book items.
  custom_book_menu_alter($items);
}
