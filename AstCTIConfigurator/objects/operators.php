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
class operators extends p4a_simple_edit_mask
{
	public function __construct()
	{
		parent::__construct("operators");
		$this->fieldset->setLabel("Details about current operator");
		
		$this->table->cols->PASS_WORD->setVisible(false);
		$this->fields->PASS_WORD->setType("password");
		
		$this->display("menu", p4a::singleton()->menu);
	}
}