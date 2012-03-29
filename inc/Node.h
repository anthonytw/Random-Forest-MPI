#ifndef __Node_h__
#define __Node_h__

#include <vector>
#include <string>
#include <sstream>

#include <iostream>

/**
 * Node interface.
 */
class Node
{
  public:
    /**
     * A set of nodes.
     */
    typedef std::vector<Node*> NodeSet;

  public:
    /**
     * Constructor.
     * @param action The action that led to the node.
     */
    Node( const std::string action ) :
      action(action)
    {
      //
    }

    /**
     * Destructor.
     */
    virtual ~Node( void )
    {
      //
    }

    /**
     * Determine the action that led to the node.
     * @return The action that led to the node.
     */
    const std::string get_action( void ) const
    {
      return action;
    }

    /**
     * Determine classification at this node.
     * @return The classification.
     */
    virtual bool get_classification( void ) const = 0;

    /**
     * Determine if the node is a leaf.
     * @return True if the node is a leaf, false otherwise.
     */
    virtual bool is_leaf( void ) const = 0;

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node ) = 0;

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void ) = 0;

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const = 0;

  private:
    const std::string action; ///< The action that led to the node.
};

/**
 * Defines a leaf (decision) node.
 */
class LeafNode : public Node
{
  public:
    /**
     * Constructor.
     * @param action The action that led to the node.
     * @param classification The classification decision of the node.
     */
    LeafNode(
      const std::string action,
      const bool classification ) :
        Node(action),
        classification(classification)
    {
      //
    }

    /**
     * Determine the classification.
     * @return The classification.
     */
    virtual bool get_classification( void ) const
    {
      return classification;
    }

    /**
     * Determine if the node is a leaf or not.
     * @return True, since this is a leaf node.
     */
    virtual bool is_leaf( void ) const
    {
      return true;
    }

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node )
    {
      // Do nothing.
    }

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void )
    {
      NodeSet set;
      return set;
    }

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const
    {
      std::string draw_string =
        "shape=ellipse,label=\"(" + get_action() +
        ")\\n" + (classification ? "1" : "0") +"\"";
      return draw_string;
    }

  private:
    const bool classification;  ///< Node classification.
};

/**
 * Defines a split node.
 */
class SplitNode : public Node
{
  public:
    /**
     * Constructor.
     * @param action The action the led to the node.
     * @param column The split column.
     * @param threshold The split threshold.
     */
    SplitNode(
      const std::string action,
      const unsigned int column,
      const double threshold ) :
        Node(action),
        column(column),
        threshold(threshold)
    {
      //
    }

    /**
     * Destructor.
     * @note The destructor will delete all of the children.
     */
    virtual ~SplitNode( void )
    {
      if ( children.size() > 0 )
      {
        for (
          NodeSet::iterator iter = children.begin();
          iter != children.end(); ++iter )
        {
          delete *iter;
        }
      }
    }

    /**
     * Determine the classification.
     * @return The classification.
     * @note This function does not return a useful value.
     */
    virtual bool get_classification( void ) const
    {
      return false;
    }

    /**
     * Determine if the node is a leaf or not.
     * @return False, since this is not a leaf node.
     */
    virtual bool is_leaf( void ) const
    {
      return false;
    }

    /**
     * Add a child to the node.
     * @param node The child node to add.
     */
    virtual void add_child( Node * const node )
    {
      // Add child.
      children.push_back( node );
    }

    /**
     * Get the children.
     * @return A list of all the node children.
     */
    virtual NodeSet get_children( void )
    {
      return children;
    }

    /**
     * Draw the node.
     * @return the GraphViz representation of the node.
     */
    virtual std::string draw( void ) const
    {
      std::stringstream ss;
      ss << column;
      std::string draw_string =
        "shape=box,label=\"(" + get_action() +
        ")\\n" + ss.str() +"\"";
      return draw_string;
    }

    /**
     * Get the split column.
     * @return The split column.
     */
    unsigned int get_column( void ) const
    {
      return column;
    }

    /**
     * Get the split threshold.
     * @return The split threshold.
     */
    double get_threshold( void ) const
    {
      return threshold;
    }

  private:
    NodeSet children;             ///< The child nodes.
    const unsigned int column;    ///< The split column.
    const double threshold;       ///< The split threshold.
};

#endif
