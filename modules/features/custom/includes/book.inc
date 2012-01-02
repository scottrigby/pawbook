<?php

/**
 * @file
 * Functions for Book module.
 */

/**
 * Pseudo-hook function for hook_menu_alter().
 *
 * @see custom_menu_alter()
 */
function custom_book_menu_alter(&$items) {
  // Disable core book outline tab.
  $items['node/%node/outline']['access callback'] = FALSE;
}

/**
 * Pseudo-hook function for hook_menu().
 *
 * @see custom_menu()
 */
function custom_book_menu() {
  $items = array();
  // Custom reorder book tab. We don't need the BookMadeSimple module for this.
  $items['node/%node/reorder'] = array(
    'title' => t('Reorder book'),
    'page callback' => 'drupal_get_form',
    'page arguments' => array('custom_book_reorder', 1),
    'access callback' => 'custom_book_reorder_access',
    'access arguments' => array(1),
    'type' => MENU_LOCAL_TASK,
    'weight' => 2,
  );
  return $items;
}

/**
 * Book reorder menu access callback.
 */
function custom_book_reorder_access($node) {
  $is_book = isset($node->book) && $node->book['bid'];
  $user_access = _book_outline_access($node);
  return $is_book && $user_access;
}

/**
 * Book reorder menu callback.
 */
function custom_book_reorder($form, $form_state, $node) {
  module_load_include('inc', 'book', 'book.admin');
  // @todo why does reordering not save at this path?
  #return drupal_get_form('book_admin_edit', node_load($node->book['bid']));
  // Stick with drupal_goto() for now.
  $query = array('destination' => "node/$node->nid");
  drupal_goto("admin/content/book/{$node->book['bid']}", array('query' => $query));
}

/**
 * Pseudo-hook function for hook_admin_paths_alter().
 *
 * @see custom_admin_paths_alter()
 */
function custom_book_admin_paths_alter(&$paths) {
  // Treat the reorder book form as administrative.
  $paths['node/*/reorder'] = TRUE;
}
