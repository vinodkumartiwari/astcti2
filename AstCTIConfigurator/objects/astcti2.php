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

/**
 * @author Fabrizio Balliano <fabrizio.balliano@crealabs.it>
 */
class astcti2 extends p4a
{
	/**
	 * @var P4A_Menu
	 */
	public $menu = null;
	
	public function __construct()
	{
		parent::__construct();
		
		$this->build("p4a_menu", "menu");
		
		$this->menu->addItem("services")
			->implement("onclick", $this, "menuClick");
			
		$this->menu->addItem("operators")
			->implement("onclick", $this, "menuClick");
			
		$this
			->setTitle("AsteriskCTI")
			->openMask("services");
	}
	
	public function menuClick()
	{
		$this->openMask($this->active_object->getName());
	}
}