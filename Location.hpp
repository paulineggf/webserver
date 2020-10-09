/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trbonnes <trbonnes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 12:24:19 by trbonnes          #+#    #+#             */
/*   Updated: 2020/10/09 15:26:31 by trbonnes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>
# include <list>

// Les locations doivent être triées par ordre de longueur \
    Du plus grand au plus petit

class   Location
{
    public:
    
    std::string                 _location;
    std::string                 _root;
    std::vector<std::string>    _allow;
    std::list<std::string>      _index;
    std::string                 _type;
    std::string                 _charset;
    std::vector<std::string>    _language;
    std::string                 _auth_basic;
    std::string                 _auth_basic_user_file;
    std::string                 _autoindex;

    std::string                 _alias;

    Location(std::string location, std::string root, std::vector<std::string> allow, 
    std::list<std::string> index, std::string type, std::string charset, std::vector<std::string> language,
    std::string auth_basic, std::string auth_basic_user_file, std::string autoindex);
    ~Location();

    Location();
    private:

};

#endif