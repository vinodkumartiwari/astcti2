<?php
/**
 * This file is part of AstCTIConfigurator.
 * 
 * AstCTIConfigurator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * AstCTIConfigurator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Fabrizio Balliano <fabrizio.balliano@crealabs.it>
 * @license http://www.gnu.org/licenses/gpl.html GNU General Public License
 */

define("P4A_LOCALE", 'en_US');
define("P4A_DSN", 'sqlite:/asteriskcti.db3');

define("P4A_AJAX_ENABLED", false);

require_once dirname(__FILE__) . '/p4a/p4a.php';

p4a::singleton("astcti2")->main();