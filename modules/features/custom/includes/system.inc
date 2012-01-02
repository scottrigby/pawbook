<?php

/**
 * @file
 * Functions for System module.
 */

/**
 * Implements hook_admin_paths_alter().
 */
function custom_admin_paths_alter(&$paths) {
  // Custom book paths.
  custom_book_admin_paths_alter($paths);
}
